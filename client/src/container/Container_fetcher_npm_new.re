module Demoboard_binding = {
  [@bs.module "@frontarm/demoboard-worker"]
  external get_package: (~name: string, ~version: string) => Js.Promise
}
let npm_pattern = [%re
  {|/^(?:npm:\/\/)?((?:@[\w.-]+\/)?\w[\w.-]+)(@[^/]+)?(\/.*)?$/|}
];˝

type npm_matches = {
  name: string,
  version: string,
  pathname: option(string),
};

external unsafe_undefined_to_option: string => option(string) = "%identity";

let handle_npm = (~url as _, ~pathname, ~meta as _) => {
  let matches = switch (pathname->String.match(npm_pattern)) {
  | Some([|_, name, version, pathname, _, _, _|]) =>
    Result.Ok({name, version, pathname: pathname->unsafe_undefined_to_option})
  | _ => Result.Error(`UnknownNpmPath)
  };
  matches -> Result.flatMapOk(({name, version, pathname}) => {

  })
};
