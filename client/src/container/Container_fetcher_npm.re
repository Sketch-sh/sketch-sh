open Utils;
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

  let parse = fullpath => {
    let slash_pos = fullpath |> Js.String.indexOf("/");
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

module D = Decode.AsResult.OfParseError;

module Jsdelivr = {
  let errorTransformer = Js.String.make;

  let api_base = "https://data.jsdelivr.com/v1";
  let cdn_base = "https://cdn.jsdelivr.net/";
  module List_file = {
    type t = {
      default_file: string,
      files: array(string),
    };

    /** This fetch a list of all files in a package and the default file */
    let fetch = slug => {
      let pkg_slug = slug->Pkg.Slug.to_string;
      // https://data.jsdelivr.com/v1/package/npm/jquery@3.2.1/flat
      let url = Url.join([|api_base, "package/npm", pkg_slug, "/flat"|]);

      let make = (default_file, files) => {
        switch (default_file) {
        | Some(default_file) => Belt.Result.Ok({default_file, files})
        | None =>
          if (files |> Js.Array.indexOf("/index.js") == (-1)) {
            Belt.Result.Error(`Npm_fetcher_cant_resolve_main_file(pkg_slug));
          } else {
            Belt.Result.Ok({default_file: "/index.js", files});
          }
        };
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
  let get_dependencies_range = slug => {
    // cdn_base/npm/thangngoc89@1.0.0/package.json
    let url =
      [|cdn_base, "npm", slug->Pkg.Slug.to_string, "package.json"|]->Url.join;
    let decoder = D.optionalField("dependencies", D.dict(D.string));
    FFetch_decode.json(url, decoder)
    ->Future.flatMapOk(deps =>
        deps
        ->Belt.Option.getWithDefault(Js.Dict.empty())
        ->Belt.Result.Ok
        ->Future.value
      );
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
    default_file: string,
    files: array(string),
    dependencies: Js.Dict.t(string),
  };
  let cache = Map.make();

  let set = (~slug, ~pkg, ~default_file, ~files, ~dependencies) => {
    cache->Map.set(
      slug->Pkg.Slug.to_string,
      {pkg, default_file, files, dependencies},
    );
  };
  let get = (~slug) => cache->Map.get(slug->Pkg.Slug.to_string);
};

let get_meta = pkg => {
  let orginal_pkg_slug = pkg->Pkg.Slug.of_pkg;
  switch (Cache.get(~slug=orginal_pkg_slug)) {
  | Some(cache) => Future.value(Belt.Result.Ok(cache))
  | None =>
    Future.value(pkg)
    ->Future.flatMap(pkg =>
        switch (pkg.version) {
        | Absolute(_) => Future.value(Belt.Result.Ok(pkg))
        | Range(_) =>
          Jsdelivr.resolve_latest(pkg->Pkg.Slug.of_pkg)
          ->Future.flatMapOk(absolute_version =>
              {...pkg, version: Absolute(absolute_version)}
              ->Belt.Result.Ok
              ->Future.value
            )
        }
      )
    ->Future.flatMapOk(pkg => {
        let slug = pkg->Pkg.Slug.of_pkg;

        let list_file = slug->Jsdelivr.List_file.fetch;
        let deps_range = slug->Jsdelivr.get_dependencies_range;

        Future.mapOk2(list_file, deps_range, (list_file, deps_range) =>
          (pkg, list_file, deps_range)
        );
      })
    ->Future.flatMapOk(value => {
        let (pkg, {Jsdelivr.List_file.default_file, files}, deps_range) = value;
        let new_pkg_slug = pkg->Pkg.Slug.of_pkg;
        Cache.set(
          ~pkg,
          ~slug=new_pkg_slug,
          ~default_file,
          ~files,
          ~dependencies=deps_range,
        );
        if (orginal_pkg_slug != new_pkg_slug) {
          Cache.set(
            ~pkg,
            ~slug=orginal_pkg_slug,
            ~default_file,
            ~files,
            ~dependencies=deps_range,
          );
        };
        {Cache.pkg, default_file, files, dependencies: deps_range}
        ->Belt.Result.Ok
        ->Future.value;
      })
  };
};

external unsafe_reject: string => 'a = "%identity";

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

let handle_npm = (~url, ~meta, ~pathname) => {
  let pkg = Pkg.parse(pathname);
  Js.log4("[npm] fetching: ", pathname, meta, pkg->Pkg.to_path);

  let fetch_result:
    Future.t(
      Belt.Result.t(
        Container_polestar.Fetcher.FetchResult.t,
        [> FFetch_decode.error],
      ),
    ) =
    get_meta(pkg)
    ->Future.flatMapOk(cache => {
        // Note: Cache.pkg path usually is incorrect, you need to relies on original pkg instead
        let {Cache.pkg: _, default_file, files, dependencies} = cache;

        /**
         * Path resolver
         * - No path -> use default_file
         * - Check if given file is in `files`
         * - If not, try to add `.js` to it
         */
        let file_path = {
          switch (pkg.path) {
          | None => Some(default_file)
          | Some(file_path) =>
            let file_path = Url.join([|"/", file_path|]);
            if (files->array_has(file_path)) {
              Some(file_path);
            } else if (files->array_has(file_path ++ ".js")) {
              Some(file_path ++ ".js");
            } else {
              None;
            };
          };
        };

        let file_path =
          switch (file_path) {
          | None =>
            Belt.Result.Error(
              `Npm_fetcher_cant_resolve_file_path(pkg->Pkg.to_path),
            )
          | Some(a) => Belt.Result.Ok(a)
          };

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
              ~dependencies=Array(Container_require_collector.parse(code)),
              ~dependencyVersionRanges=dependencies,
              ~code,
              (),
            )
            ->Belt.Result.Ok
            ->Future.value
          );
      });

  Js.Promise.make((~resolve, ~reject) =>
    fetch_result
    ->Future.map(result =>
        switch (result) {
        | Belt.Result.Ok(result) => resolve(. result)
        | Belt.Result.Error(error) =>
          reject(. unsafe_reject(error_handle(error)))
        }
      )
    ->ignore
  );
};
