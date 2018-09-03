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
  link:
    (. Editor_Types.lang, string, string) =>
    Js.Promise.t(Worker_Types.linkResult),
  linkMany:
    (. list(Editor_Types.Link.link)) => Js.Promise.t(Worker_Types.linkResult),
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
let link = toplevel->linkGet;
let linkMany = toplevel->linkManyGet;
let refmtMany = toplevel->refmtManyGet;
