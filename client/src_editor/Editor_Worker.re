open Worker_Binding;

module ToplevelWorker = {
  [@bs.new] [@bs.module]
  external make: unit => Worker.t = "../src_analyzer/Worker_Index.bs.js";
};

[@bs.deriving abstract]
type js_executeResult = {
  evaluate: string,
  stderr: string,
  stdout: string,
};

[@bs.deriving abstract]
type toplevel = {
  execute: (. bool, string) => Js.Promise.t(list(Worker_Types.blockData)),
  executeMany:
    (. Editor_Types.lang, list((string, string))) =>
    Js.Promise.t(list((string, list(Worker_Types.blockData)))),
  reToMl:
    (. string) =>
    Js.Promise.t(Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error)),
  reToRe:
    (. string) =>
    Js.Promise.t(Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error)),
  mlToRe:
    (. string) =>
    Js.Promise.t(Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error)),
};
let worker = ToplevelWorker.make();
let toplevel: toplevel = Comlink.comlink->(Comlink.proxy(worker));

let execute = toplevel->executeGet;
let executeMany = toplevel->executeManyGet;

let reToMl = toplevel->reToMlGet;
let reToRe = toplevel->reToReGet;
let mlToRe = toplevel->mlToReGet;
