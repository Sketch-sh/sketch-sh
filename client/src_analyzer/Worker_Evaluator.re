module Types = {
  [@bs.deriving abstract]
  type pos = {
    line: int,
    col: int,
  };
  type loc = (pos, pos);

  module PhraseContent = {
    [@bs.deriving abstract]
    type t = {
      loc: Js.Nullable.t(loc),
      value: string,
      stderr: string,
      stdout: string,
    };
  };

  type phraseResult = Belt.Result.t(PhraseContent.t, PhraseContent.t);

  [@bs.deriving abstract]
  type js_phraseResult = {
    kind: string,
    value: PhraseContent.t,
  };

  type execResult = array(js_phraseResult);
};

module type EvaluatorSig = {
  let js_execute: string => Types.execResult;
  let js_reset: unit => unit;
};

module Make = (B: EvaluatorSig) => {
  include Types;
  let reset = B.js_reset;
  let execute = code => B.js_execute(code);
};
