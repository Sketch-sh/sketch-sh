open Worker_Binding;

module RtopWorker = {
  [@bs.new] [@bs.module]
  external make : unit => Worker.t = "../src_analyzer/Worker_Index.bs.js";
};

[@bs.deriving abstract]
type js_executeResult = {
  evaluate: string,
  stderr: string,
  stdout: string,
};

[@bs.deriving abstract]
type rtop = {
  execute:
    (. string) =>
    Js.Promise.t(list(Worker_Types.wholeProgramExecuteResult)),
  /* reset: (. unit) => Js.Promise.t(unit),
     reasonSyntax: (. unit) => Js.Promise.t(unit),
     mlSyntax: (. unit) => Js.Promise.t(unit),
     parseError: (. string, string) => Js.Promise.t(string), */
};
let worker = RtopWorker.make();

let rtop: rtop = Comlink.comlink |. Comlink.proxy(worker);

let execute = rtop |. executeGet;
