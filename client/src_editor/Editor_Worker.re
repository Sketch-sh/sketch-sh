open Worker_Binding;

module RtopWorker = {
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
type rtop = {
  execute: (. bool, string) => Js.Promise.t(list(Worker_Types.blockData)),
  executeMany:
    (. list((string, Editor_Types.Block.lang, string))) =>
    Js.Promise.t(list((string, list(Worker_Types.blockData)))),
};
let worker = RtopWorker.make();

let rtop: rtop = Comlink.comlink->(Comlink.proxy(worker));

let execute = rtop->executeGet;

let executeMany = rtop->executeManyGet;
