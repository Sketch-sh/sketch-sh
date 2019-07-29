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
    let splitted = fullpath |> Js.String.splitAtMost("/", ~limit=1);
    let (name, version) = splitted[0]->parse_slug;
    let version = version->Version.of_string;

    switch (splitted->Belt.Array.length) {
    | 1 => {name, version, path: None}
    | _ => {name, version, path: Some(splitted[1])}
    };
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
      // https://data.jsdelivr.com/v1/package/npm/jquery@3.2.1/flat
      let url =
        Url.join([|
          api_base,
          "package/npm",
          slug->Pkg.Slug.to_string,
          "/flat",
        |]);

      let decoder = json => {
        D.Pipeline.(
          succeed((default_file, files) => {default_file, files})
          |> field("default", string)
          |> field("files", array(D.field("name", D.string)))
          |> run(json)
        );
      };

      FFetch_decode.json(url, decoder);
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
    FFetch.text(url)
    ->Future.flatMapOk(file_content =>
        (url, file_content)->Belt.Result.Ok->Future.value
      );
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

let handle_npm = (~url, ~meta as _, ~pathname) => {
  let pkg = Pkg.parse(pathname);
  Js.log2("[npm] fetching: ", pathname);

  let fetch_result:
    Future.t(
      Belt.Result.t(
        Container_polestar.Fetcher.FetchResult.t,
        [> FFetch_decode.error],
      ),
    ) =
    get_meta(pkg)
    ->Future.flatMapOk(cache => {
        // TODO: Check in files instead of sending request
        let {Cache.pkg, default_file, files: _, dependencies} = cache;

        let file_path = pkg.path->Belt.Option.getWithDefault(default_file);

        Jsdelivr.get_file(pkg, file_path)
        ->Future.flatMapOk(((id, code)) =>
            Container_polestar.Fetcher.FetchResult.make(
              ~url,
              ~id,
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
        | Belt.Result.Ok(result) =>
          Js.log(result);
          resolve(. result);
        | Belt.Result.Error(error) =>
          reject(. unsafe_reject(error->FFetch_decode.error_to_string))
        }
      )
    ->ignore
  );
};
