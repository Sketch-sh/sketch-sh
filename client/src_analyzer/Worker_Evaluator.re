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
  [@bs.deriving abstract]
  type js_phraseResult = {
    kind: string,
    value: PhraseContent.t,
  };
  type execResult = array(js_phraseResult);

  module Refmt = {
    type implementationAST;
    type interfaceAST;

    type error = {
      loc: Js.Nullable.t(loc),
      message: string,
    };

    type refmtManyResult = list((Editor_Types.id, string, option(error)));
  };

  module LinkResult = {
    [@bs.deriving abstract]
    type js_linkResult = {
      kind: string,
      value: string,
    };
  };
};

module type EvaluatorSig = {
  let js_execute: string => Types.execResult;
  let js_reset: unit => unit;
  let mlSyntax: unit => unit;
  let reSyntax: unit => unit;

  let parseRE: string => Types.Refmt.implementationAST;
  let parseREI: string => Types.Refmt.interfaceAST;
  let printRE: Types.Refmt.implementationAST => string;
  let printREI: Types.Refmt.interfaceAST => string;

  let parseML: string => Types.Refmt.implementationAST;
  let parseMLI: string => Types.Refmt.interfaceAST;
  let printML: Types.Refmt.implementationAST => string;
  let printMLI: Types.Refmt.interfaceAST => string;
  let insertModule:
    (. string, string, string) => Types.LinkResult.js_linkResult;
};

module Make = (B: EvaluatorSig) => {
  include Types;
  let reset = B.js_reset;
  let execute = code => B.js_execute(code);
  let mlSyntax = B.mlSyntax;
  let reSyntax = B.reSyntax;

  external unsafeAsError: Js.Exn.t => Refmt.error = "%identity";
  let _wrap: ('a => 'b, 'a) => Belt.Result.t('b, Refmt.error) =
    (f, x) =>
      try (Ok(f(x))) {
      | Js.Exn.Error(exn) => Error(unsafeAsError(exn))
      };

  let parseRE = B.parseRE->_wrap;
  let parseREI = B.parseREI->_wrap;
  let parseML = B.parseML->_wrap;
  let parseMLI = B.parseMLI->_wrap;

  let printRE = B.printRE;
  let printREI = B.printREI;
  let printML = B.printML;
  let printMLI = B.printMLI;
  let insertModule =
    (. name, content, lang) => B.insertModule(. name, content, lang);
};
