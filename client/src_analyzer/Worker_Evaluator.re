[@bs.deriving abstract]
type position = {
  line: int,
  col: int,
};
type location = (position, position);

[@bs.deriving abstract]
type phraseContent = {
  loc: Js.Nullable.t(location),
  value: string,
  stderr: string,
  stdout: string,
};

type phraseResult = Belt.Result.t(phraseContent, phraseContent);
type execResult = array(phraseResult);

module type EvaluatorSig = {
  let js_execute: string => execResult;
  let js_reset: unit => unit;
};

module Make = (B: EvaluatorSig) => {
  let reset = B.js_reset;
  let execute = code => B.js_execute(code);
};
