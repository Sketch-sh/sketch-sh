[@bs.deriving abstract]
type js_executeResult = {
  evaluate: string,
  stderr: string,
  stdout: string,
};

[@bs.deriving abstract]
type evalutator = {execute: string => js_executeResult};

[@bs.module "../../public/reason.js"] external evalutator : evalutator = "evaluator";
let js_execute = executeGet(evalutator);

[@bs.deriving abstract]
type refmterr = {parse: (~content: string, ~error: string) => string};

[@bs.module "../../public/reason.js"] external refmterr : refmterr = "refmterr";
let js_parseError = parseGet(refmterr);

let parseError = (~content, ~error) =>
  js_parseError(~content, ~error=error |> Js.String.replace({|File ""|}, {|File "_none_"|}));

type executeResult =
  | Error(string)
  | Ok(string)
  | OkWithLog(string, string);

let execute = code => {
  let result = js_execute(code);
  let stderr = result |. stderrGet |. Js.String.trim;
  let stdout = result |. stdoutGet |. Js.String.trim;
  let evaluate = result |. evaluateGet |. Js.String.trim;
  switch (stderr == "", stdout == "", evaluate == "") {
  | (false, true, true) => Error(stderr)
  | (true, true, false) => Ok(evaluate)
  | (true, false, false) => OkWithLog(evaluate, stdout)
  | _ =>
    Js.log(result);
    Invalid_argument("What the heck is going on with this code? " ++ code) |. raise;
  };
};
