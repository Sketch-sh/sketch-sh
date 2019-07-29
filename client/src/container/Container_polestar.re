type versionRanges = Js.Dict.t(string);

module Fetcher = {
  type meta = {
    .
    "requiredById": Js.Nullable.t(string),
    "originalRequest": Js.Nullable.t(string),
  };

  type url = string;

  type dependencies =
    | Umd
    | Array(array(string));

  module FetchResult: {
    type t;

    let make:
      (
        ~id: string,
        ~url: string,
        ~dependencies: dependencies=?,
        ~dependencyVersionRanges: versionRanges=?,
        ~css: string=?,
        ~code: string=?,
        unit
      ) =>
      t;
  } = {
    type js_dependencies;

    [@bs.deriving abstract]
    type t = {
      id: string,
      url: string,
      [@bs.optional]
      dependencies: js_dependencies,
      [@bs.optional]
      code: string,
      [@bs.optional]
      dependencyVersionRanges: versionRanges,
      [@bs.optional]
      css: string,
    };
    external unsafe_make_js_deps: 'a => js_dependencies = "%identity";
    let make =
        (
          ~id,
          ~url,
          ~dependencies=?,
          ~dependencyVersionRanges=?,
          ~css=?,
          ~code=?,
          (),
        ) => {
      t(
        ~id,
        ~url,
        ~dependencies=?
          dependencies->Belt.Option.map(
            fun
            | Umd => unsafe_make_js_deps("umd")
            | Array(array) => unsafe_make_js_deps(array),
          ),
        ~dependencyVersionRanges?,
        ~css?,
        ~code?,
        (),
      );
    };
  };
};

type polestar;

type resolver;
type polestar_opts('globals, 'moduleThis) = {
  .
  "globals": Js.t('globals),
  "moduleThis": 'moduleThis,
  "fetcher": (. string, Fetcher.meta) => Js.Promise.t(Fetcher.FetchResult.t),
  "resolver": resolver,
  "onEntry": (. unit) => unit,
  "onError": (. exn) => unit,
};

[@bs.module "polestar"] [@bs.new]
external make: polestar_opts('a, 'b) => polestar = "Polestar";

[@bs.module "polestar"] [@bs.new]
external defaultResolver: resolver = "DefaultResolver";

type polestar_module;

[@bs.send]
external require: (polestar, string) => Js.Promise.t(polestar_module) =
  "require";

[@bs.send]
external evaluate:
  (polestar, array(string), string) => Js.Promise.t(polestar_module) =
  "evaluate";
