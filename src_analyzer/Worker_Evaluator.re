[@bs.deriving abstract]
type js_executeResult = {
  evaluate: string,
  stderr: string,
  stdout: string,
};

module type EvaluatorSig = {let js_execute: string => js_executeResult;};

module Make = (B: EvaluatorSig) => {
  open Worker_Types;

  let emptyStringToOption =
    fun
    | "" => None
    | str => Some(str);

  let execute = code => {
    let result = B.js_execute(code);
    {
      evaluate:
        result |. evaluateGet |. Js.String.trim |. emptyStringToOption,
      stderr: result |. stderrGet |. Js.String.trim |. emptyStringToOption,
      stdout: result |. stdoutGet |. Js.String.trim |. emptyStringToOption,
    };
  };
};
