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

open Editor_Types;

[@bs.deriving abstract]
type toplevel = {
  execute: (. bool, string) => Js.Promise.t(list(Worker_Types.blockData)),
  executeMany:
    (. lang, list((id, string))) =>
    Js.Promise.t(list((id, list(Worker_Types.blockData)))),
  refmtMany:
    (. lang, list((id, string)), bool) =>
    Js.Promise.t(Worker_Evaluator.Types.Refmt.refmtManyResult),
};

let worker = ToplevelWorker.make();
let toplevel: toplevel = Comlink.comlink->(Comlink.proxy(worker));

let execute = toplevel->executeGet;
let executeMany = toplevel->executeManyGet;
let refmtMany = toplevel->refmtManyGet;
