// TODO: move this into web worker
[@bs.module]
external konan:
  string =>
  {
    .
    "expressions": array(string),
    "strings": array(string),
  } =
  "konan";

let parse = code => {
  let raw = konan(code);
  raw##strings;
};
