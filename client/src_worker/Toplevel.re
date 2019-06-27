[%%debugger.chrome];

module Types = {
  type blockInput = {
    binput_id: string,
    binput_value: string,
  };
  type blockResult = {
    id: string,
    result: list(Worker_Types.blockData),
  };
  type linkResult = {
    link: Editor_Types.Link.link,
    result: Worker_Types.linkResult,
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
    | Execute(
        Editor_Types.lang,
        list(blockInput),
        list(Editor_Types.Link.link),
      )
    | ExecuteEmbed(Editor_Types.lang, string)
    | Refmt(refmtTypes, list(blockInput))
    | LoadScript(string);

  type topToWorkerData = {
    t_id: string,
    t_message: topToWorkerMessage,
  };

  type workerToTopMessage =
    | Ready
    | ExecuteResult(
        Belt.Result.t((list(linkResult), list(blockResult)), string),
      )
    | ExecuteEmbedResult(
        Belt.Result.t(list(Worker_Types.blockData), string),
      )
    | RefmtResult(Belt.Result.t(refmtOk, string))
    | LoadScriptResult;

  type workerToTopData = {
    w_id: string,
    w_message: workerToTopMessage,
  };

  [@bs.new] [@bs.module]
  external make: unit => Worker.worker = "./Toplevel_Worker.bs.js";
};

include Worker.Make(Types);
