module CompilerErrorMessage = {
  /*
   Ocaml's parse location is really weird:
   - Line: 1-based
   - Col: 0-based
   */
  type pos = Worker_Evaluator.Types.pos;
  let mkPos = Worker_Evaluator.Types.pos;
  type content = {
    o_content: string,
    o_loc: (pos, pos),
  };

  type t =
    | Err_Warning(content)
    | Err_Error(content)
    | Err_Unknown(string);
};

type pos = {
  line: int,
  col: int,
};

module ErrorMessage = {
  type content = {
    errMsg_content: string,
    errMsg_loc: (pos, pos),
  };

  type t =
    | Err_Warning(content)
    | Err_Error(content)
    | Err_Unknown(string);
};

type blockResult = {
  blockResult_evaluate: option(string),
  blockResult_stderr: option(array(ErrorMessage.t)),
  blockResult_stdout: option(string),
};

type blockData = {
  block_result: blockResult,
  block_loc: (pos, pos),
};
