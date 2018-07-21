[@bs.deriving abstract]
type js_executeResult = {
  evaluate: string,
  stderr: string,
  stdout: string,
};

[@bs.val] external js_execute : string => js_executeResult = "evaluator.execute";

type executeResult =
  | Error(string)
  | Ok(string)
  | OkWithLog(string, string)
  | OkWithError(string, string)
  | Log(string)
  | Nothing;

let execute = code => {
  let result = js_execute(code);
  let stderr = result |. stderrGet |. Js.String.trim;
  let stdout = result |. stdoutGet |. Js.String.trim;
  let evaluate = result |. evaluateGet |. Js.String.trim;
  switch (stderr == "", stdout == "", evaluate == "") {
  /* no error, no output, no evaluate */
  | (false, true, true) => Error(stderr)
  | (true, true, false) => Ok(evaluate)
  | (true, false, false) => OkWithLog(evaluate, stdout)
  | (false, true, false) => OkWithError(evaluate, stderr)
  | (true, false, true) => Log(stdout)
  | (true, true, true) => Nothing
  | _ =>
    Js.log(result);
    Invalid_argument("What the heck is going on with this code? " ++ code) |. raise;
  };
};

[@bs.val] external reset : unit => unit = "evaluator.reset";
[@bs.val] external reasonSyntax : unit => unit = "evaluator.reasonSyntax";
[@bs.val] external mlSyntax : unit => unit = "evaluator.mlSyntax";

[@bs.val] external js_parseError : (~content: string, ~error: string) => string = "berror.parse";

let parseError = (~content, ~error) =>
  js_parseError(~content, ~error=error |> Js.String.replace({|File ""|}, {|File "_none_"|}));
