module Version: {
  type t =
    | Range(string)
    | Absolute(string);
  let of_string: string => t;
  let to_string: t => string;
} = {
  type t =
    | Range(string)
    | Absolute(string);

  let absolute_re = [%re "/^(\\d+)\\.(\\d+)\\.(\\d+)$/"];
  let is_absolute = version => absolute_re->Js.Re.test_(version);

  let of_string = version => {
    is_absolute(version) ? Absolute(version) : Range(version);
  };
  let to_string =
    fun
    | Range(v)
    | Absolute(v) => v;
};

module Pkg = {
  type t = {
    name: string,
    version: Version.t,
    path: option(string),
  };

  let parse_slug = slug => {
    let sep = slug |> Js.String.lastIndexOf("@");
    (
      slug |> Js.String.slice(~from=0, ~to_=sep),
      slug |> Js.String.sliceToEnd(~from=sep + 1),
    );
  };

  let nth_index: (string, string, int) => int = [%raw
    {|
    function nthIndex(str, pat, n){
    var L= str.length, i= -1;
    while(n-- && i++<L){
        i= str.indexOf(pat, i);
        if (i < 0) break;
    }
    return i;
}
  |}
  ];
  let parse = fullpath => {
    let slash_pos =
      if (fullpath |> Js.String.startsWith("@")) {
        nth_index(fullpath, "/", 2);
      } else {
        fullpath |> Js.String.indexOf("/");
      };
    let (slug, path) =
      switch (slash_pos) {
      | (-1) => (fullpath, None)
      | pos =>
        let slug = fullpath |> Js.String.slice(~from=0, ~to_=pos);
        let path = fullpath |> Js.String.sliceToEnd(~from=pos + 1);
        (
          slug,
          switch (path |> Js.String.length) {
          | 0 => None
          | _ => Some(path)
          },
        );
      };

    let (name, version) = parse_slug(slug);
    let version = version->Version.of_string;
    {name, version, path};
  };

  module Slug: {
    type slug;
    let of_pkg: t => slug;
    let to_string: slug => string;
  } = {
    type slug = string;
    let of_pkg = pkg => pkg.name ++ "@" ++ Version.to_string(pkg.version);
    let to_string = a => a;
  };

  let get_slug = pkg => pkg->Slug.of_pkg->Slug.to_string;

  let to_path = pkg => {
    let slug = get_slug(pkg);
    let base_path = {j|npm://$(slug)|j};

    Url.join([|base_path, pkg.path->Belt.Option.getWithDefault("")|]);
  };
};

module Pkg_spec = {
  /**
   * This describe and specs of the currently package
   * Currently supports:
   *   - Commonjs
   *   - Umd(path_to_umd_bundle)
   */
  type t =
    | Commonjs
    | Umd(string);

  let make = pkg => {
    let is_umd =
      Js.Dict.get(Container_fetcher_npm_setting.umd_pathname, pkg.Pkg.name);

    switch (is_umd) {
    | None => Commonjs
    | Some(umd_bundle) => Umd(umd_bundle)
    };
  };
};

module D = Decode.AsResult.OfParseError;

module Jsdelivr = {
  let errorTransformer = Js.String.make;

  let api_base = "https://data.jsdelivr.com/v1";
  let cdn_base = "https://cdn.jsdelivr.net/";
  module List_file = {
    type t = {
      default_file: option(string),
      files: array(string),
    };

    /** This fetch a list of all files in a package and the default file */
    let fetch = slug => {
      let pkg_slug = slug->Pkg.Slug.to_string;
      // https://data.jsdelivr.com/v1/package/npm/jquery@3.2.1/flat
      let url = Url.join([|api_base, "package/npm", pkg_slug, "/flat"|]);

      let make = (default_file, files) => {
        Result.Ok({default_file, files});
      };

      let decoder = json => {
        D.Pipeline.(
          succeed((a, b) => (a, b))
          |> optionalField("default", string)
          |> field("files", array(D.field("name", D.string)))
          |> run(json)
        );
      };

      FFetch_decode.json(url, decoder)
      ->Future.flatMapOk(((a, b)) => make(a, b)->Future.value);
    };
  };

  /** Resolve relative slug into full version range */
  let resolve_latest = (slug: Pkg.Slug.slug) => {
    // api_base/package/resolve/npm/:name@:range
    let url =
      Url.join([|api_base, "package/resolve/npm", slug->Pkg.Slug.to_string|]);
    let decoder = D.field("version", D.string);
    FFetch_decode.json(url, decoder);
  };

  /** Get all dependencies range of a package inside package.json */
  type pkg_json = {
    dependencies: Js.Dict.t(string),
    browser: Js.Dict.t(string),
  };

  let fetch_pkg_json = slug => {
    // cdn_base/npm/thangngoc89@1.0.0/package.json
    let url =
      [|cdn_base, "npm", slug->Pkg.Slug.to_string, "package.json"|]->Url.join;
    let decode_browser = json => {
      D.dict(D.string, json)->Result.flatMapError(_ => Ok(Js.Dict.empty()));
    };
    let decoder = json => {
      D.Pipeline.(
        succeed((dependencies, browser) =>
          {
            dependencies:
              dependencies->Belt.Option.getWithDefault(Js.Dict.empty()),
            browser:
              // Get browser field and make sure all paths are relative
              browser
              ->Belt.Option.getWithDefault(Js.Dict.empty())
              ->Js.Dict.entries
              ->Belt.Array.map(((key, value)) =>
                  (Node.Path.resolve("/", key), value)
                )
              ->Js.Dict.fromArray,
          }
        )
        |> optionalField("dependencies", D.dict(D.string))
        |> optionalField("browser", decode_browser)
        |> run(json)
      );
    };
    FFetch_decode.json(url, decoder);
  };

  let get_file = (pkg, file) => {
    let url =
      [|cdn_base, "npm", pkg->Pkg.Slug.of_pkg->Pkg.Slug.to_string, file|]
      ->Url.join;
    FFetch.text(url);
  };
};

module Cache = {
  module Map = Belt.MutableMap.String;

  type t = {
    pkg: Pkg.t,
    default_file: option(string),
    files: array(string),
    dependencies: Js.Dict.t(string),
    browser: Js.Dict.t(string),
  };
  let cache = Map.make();

  let set = (~slug, ~pkg, ~default_file, ~files, ~dependencies, ~browser) => {
    cache->Map.set(
      slug->Pkg.Slug.to_string,
      {pkg, default_file, files, dependencies, browser},
    );
  };
  let get = (~slug) => cache->Map.get(slug->Pkg.Slug.to_string);
};

/**
 * This get the package metadata
 * - Check in cache first
 * - If not:
 *   + Resolve version range into absolute version
 *   + Send meta request
 */
let get_meta = pkg => {
  let orginal_pkg_slug = pkg->Pkg.Slug.of_pkg;
  switch (Cache.get(~slug=orginal_pkg_slug)) {
  | Some(cache) => Future.value(Ok(cache))
  | None =>
    let pkg =
      switch (pkg.version) {
      | Absolute(_) => Future.value(Ok(pkg))
      | Range(_) =>
        Jsdelivr.resolve_latest(pkg->Pkg.Slug.of_pkg)
        ->Future.flatMapOk(absolute_version =>
            {...pkg, version: Absolute(absolute_version)}
            ->Belt.Result.Ok
            ->Future.value
          )
      };

    pkg
    ->Future.flatMapOk(pkg => {
        let slug = pkg->Pkg.Slug.of_pkg;

        let list_file = slug->Jsdelivr.List_file.fetch;
        let fetch_pkg_json = slug->Jsdelivr.fetch_pkg_json;

        Future.mapOk2(
          list_file,
          fetch_pkg_json,
          (list_file, {Jsdelivr.dependencies: deps_range, browser}) =>
          (pkg, list_file, deps_range, browser)
        );
      })
    ->Future.flatMapOk(value => {
        let (
          pkg,
          {Jsdelivr.List_file.default_file, files},
          deps_range,
          browser,
        ) = value;
        let new_pkg_slug = pkg->Pkg.Slug.of_pkg;
        Cache.set(
          ~pkg,
          ~slug=new_pkg_slug,
          ~default_file,
          ~files,
          ~dependencies=deps_range,
          ~browser,
        );
        if (orginal_pkg_slug != new_pkg_slug) {
          Cache.set(
            ~pkg,
            ~slug=orginal_pkg_slug,
            ~default_file,
            ~files,
            ~dependencies=deps_range,
            ~browser,
          );
        };
        {Cache.pkg, default_file, files, dependencies: deps_range, browser}
        ->Belt.Result.Ok
        ->Future.value;
      });
  };
};

let fetch_umd = (~pkg, ~umd_path, ~url) => {
  Jsdelivr.get_file(pkg, umd_path)
  ->Future.flatMapOk(code =>
      Container_polestar.Fetcher.FetchResult.make(
        ~url,
        ~id={
          {...pkg, path: Some(umd_path)}->Pkg.to_path;
        },
        ~dependencies=Umd,
        ~code,
        (),
      )
      ->Belt.Result.Ok
      ->Future.value
    );
};

module Fetch_commonjs: {
  let fetch_commonjs:
    (~pkg: Pkg.t, ~url: string) =>
    Future.t(
      Belt.Result.t(
        Container_polestar.Fetcher.FetchResult.t,
        [>
          FFetch_decode.error
          | `Npm_fetcher_cant_resolve_file_path(string)
          | `Npm_fetcher_cant_resolve_main_file(string)
        ],
      ),
    );
} = {
  /**
   * Replace calls to Node's built in packages (path, fs, crypto)
   * to a package on npm
   */
  let map_node_builtin_pkg = pkg => {
    switch (
      Container_fetcher_npm_setting.package_map->Js.Dict.get(pkg.Pkg.name)
    ) {
    | Some(name) => {...pkg, name}
    | None => pkg
    };
  };

  let check_if_minified = (~file_path as path, ~files) =>
    if (path->String.indexOf(".min.js") === (-1)) {
      None;
    } else {
      let unminified_path = path->String.replace(".min.js", ".js");
      if (files->Arr.has(unminified_path)) {
        Some(path);
      } else {
        None;
      };
    };
  /**
    * Resolve the require path into module absolute path
    * Node allows you to omit `.js` extension, also `index.js` in a folder
    * - Check if given file is in `files`
    * - Add `.js` extension and retry
    * TODO: check for `index.js` if it's a folder, support `.json`
    */
  let resolve_module = (~path, ~files) => {
    let file_path = Url.join([|"/", path|]);

    if (files->Arr.has(file_path)) {
      Some(file_path);
    } else if (files->Arr.has(file_path ++ ".js")) {
      Some(file_path ++ ".js");
    } else {
      check_if_minified(~file_path, ~files);
    };
  };

  /**
   * Map give file to its browser counterpart if possible
   * Some package contains special version for usage in browser
   * It's provided in package.json/browser field
   * browser: {
   *   "original": "browser_counter_path"
   * }
   */
  let resolve_browser_module = (~path, ~browser) => {
    browser->Js.Dict.get(path)->Option.getWithDefault(path);
  };

  let fetch_commonjs:
    (~pkg: Pkg.t, ~url: string) =>
    Future.t(
      Belt.Result.t(
        Container_polestar.Fetcher.FetchResult.t,
        [> FFetch_decode.error],
      ),
    ) =
    (~pkg, ~url) => {
      let pkg = pkg->map_node_builtin_pkg;

      let fetch_result =
        pkg
        ->get_meta
        ->Future.flatMapOk(cache => {
            // Note: Cache.pkg path usually is incorrect, you need to relies on original pkg instead
            let {Cache.pkg: _, default_file, files, dependencies, browser} = cache;

            let file_path =
              pkg.path
              ->{
                  fun
                  | None => default_file
                  | Some(path) => Some(path);
                }
              ->Option.flatMap(path => resolve_module(~path, ~files))
              ->Option.map(path => resolve_browser_module(~path, ~browser))
              ->(
                  fun
                  | None =>
                    Error(
                      `Npm_fetcher_cant_resolve_file_path(pkg->Pkg.to_path),
                    )
                  | Some(a) => Ok(a)
                );

            Future.value(file_path)
            ->Future.flatMapOk(file_path =>
                Jsdelivr.get_file(pkg, file_path)
                ->Future.flatMapOk(code =>
                    (file_path, code)->Belt.Result.Ok->Future.value
                  )
              )
            ->Future.flatMapOk(((file_path, code)) =>
                Container_polestar.Fetcher.FetchResult.make(
                  ~url,
                  ~id={
                    {...pkg, path: Some(file_path)}->Pkg.to_path;
                  },
                  ~dependencies=
                    Array(Container_require_collector.parse(code)),
                  ~dependencyVersionRanges=dependencies,
                  ~code,
                  (),
                )
                ->Belt.Result.Ok
                ->Future.value
              );
          });

      fetch_result;
    };
};

let error_handle =
  fun
  | `ApiErrorJson(url, json) => {
      let error_msg = json->Js.Json.stringifyAny;
      switch (error_msg) {
      | None => {j|ApiErrorJson:\nUrl: $url\n$json|j}
      | Some(msg) => {j|ApiErrorJson:\nUrl: $url\n$msg|j}
      };
    }
  | `ApiErrorText(url, string) => {j|ApiErrorText:\nUrl: $url\n$string|j}
  | `DecodeError(err) => D.ParseError.failureToDebugString(err)
  | `NetworkError(string) => {j|NetworkError $string|j}
  | `Npm_fetcher_cant_resolve_main_file(pkg_slug) => {j|Npm fetcher: Can't resolve **main** file for package $(pkg_slug)
Try to require a full path, for example: require($(pkg_slug)/path_to_file.js)|j}
  | `Npm_fetcher_cant_resolve_file_path(pkg_slug) => {j|Npm fetcher: Can't resolve file path for package $(pkg_slug)
Try to require a full path, for example: require($(pkg_slug)/path_to_file.js)|j};

let handle_npm = (~url, ~meta as _, ~pathname) => {
  [%log.info "Handle npm"; ("path", pathname); ("url", url)];
  let pkg = Pkg.parse(pathname);

  let fetch_result =
    switch (Pkg_spec.make(pkg)) {
    | Commonjs => Fetch_commonjs.fetch_commonjs(~url, ~pkg)
    | Umd(umd_bundle) => fetch_umd(~pkg, ~umd_path=umd_bundle, ~url)
    };

  Js.Promise.make((~resolve, ~reject) =>
    fetch_result
    ->Future.map(result =>
        switch (result) {
        | Belt.Result.Ok(result) => resolve(. result)
        | Belt.Result.Error(error) =>
          reject(. Promise.unsafe_reject(error_handle(error)))
        }
      )
    ->ignore
  );
};


include Container_fetcher_npm_new;
