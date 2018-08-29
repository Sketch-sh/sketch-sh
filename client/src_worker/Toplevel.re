[%%debugger.chrome];
open Worker_Types;

module Types = {
  type topToWorkerMessage =
    | Execute(Editor_Types.lang, list((string, string)));

  type topToWorkerData = {
    t_id: string,
    t_message: topToWorkerMessage,
  };

  type blockResult = {
    id: string,
    result: list(blockData),
  };
  type workerToTopMessage =
    | Ready
    | ExecuteResult(Belt.Result.t(list(blockResult), string));

  type workerToTopData = {
    w_id: string,
    w_message: workerToTopMessage,
  };

  [@bs.new] [@bs.module]
  external make: unit => Worker.worker = "./Toplevel_Worker.bs.js";
};

include Worker.Make(Types);
