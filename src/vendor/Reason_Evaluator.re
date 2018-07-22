[@bs.deriving abstract]
type js_executeResult = {
  evaluate: string,
  stderr: string,
  stdout: string,
};

[@bs.deriving abstract]
type evalutator = {
  execute: string => js_executeResult,
  reset: unit => unit,
  reasonSyntax: unit => unit,
  mlSyntax: unit => unit,
};
[@bs.val] external evaluator : evalutator = "evaluator";

let reset = evaluator |. resetGet;
let reasonSyntax = evaluator |. reasonSyntaxGet;
let mlSyntax = evaluator |. mlSyntaxGet;
let js_execute = evaluator |. executeGet;

type executeResult = {
  evaluate: option(string),
  stderr: option(string),
  stdout: option(string),
};

let emptyStringToOption =
  fun
  | "" => None
  | str => Some(str);

let execute = code => {
  let result = code |. js_execute;
  {
    evaluate: result |. evaluateGet |. Js.String.trim |. emptyStringToOption,
    stderr: result |. stderrGet |. Js.String.trim |. emptyStringToOption,
    stdout: result |. stdoutGet |. Js.String.trim |. emptyStringToOption,
  };
};

[@bs.deriving abstract]
type berror = {parse: (~content: string, ~error: string) => string};

[@bs.val] external berror : berror = "berror";

let js_parseError = berror |. parseGet;

let parseError = (~content, ~error) =>
  js_parseError(~content, ~error=error |> Js.String.replace({|File ""|}, {|File "_none_"|}));
