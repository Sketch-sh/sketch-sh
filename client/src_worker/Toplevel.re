[%%debugger.chrome];

open Worker_Types;

module Types = {
  type blockInput = {
    binput_id: string,
    binput_value: string,
  };
  type blockResult = {
    id: string,
    result: list(blockData),
  };
  type refmtResult = {
    refmt_id: string,
    refmt_value: Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error),
  };

  type refmtOk = {
    hasError: bool,
    result: list(refmtResult),
  };

  type refmtTypes =
    | ReToMl
    | MlToRe
    | PrettyPrintRe;

  type topToWorkerMessage =
    | Link(list(Editor_Types.Link.link))
    | Execute(Editor_Types.lang, list(blockInput))
    | Refmt(refmtTypes, list(blockInput));

  type topToWorkerData = {
    t_id: string,
    t_message: topToWorkerMessage,
  };

  type workerToTopMessage =
    | Ready
    | LinkResult(list((Editor_Types.Link.link, Worker_Types.linkResult)))
    | ExecuteResult(Belt.Result.t(list(blockResult), string))
    | RefmtResult(Belt.Result.t(refmtOk, string));

  type workerToTopData = {
    w_id: string,
    w_message: workerToTopMessage,
  };

  [@bs.new] [@bs.module]
  external make: unit => Worker.worker = "./Toplevel_Worker.bs.js";
};

include Worker.Make(Types);
