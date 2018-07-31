module CompilerErrorMessage = {
  /*
   Ocaml's parse location is really weird:
   - Line: 1-based
   - Col: 0-based
   */
  type loc = {
    o_line: int,
    o_col: int,
  };
  type content = {
    o_content: string,
    o_pos: (loc, loc),
  };

  type t =
    | Err_Warning(content)
    | Err_Error(content)
    | Err_Unknown(string);
};

type singleExecuteResult = {
  evaluate: option(string),
  stderr: option(string),
  stdout: option(string),
};

type loc = {
  line: int,
  col: int,
};

type wholeProgramExecuteResult = {
  buffer: string,
  executeResult: singleExecuteResult,
  pos: (loc, loc),
};

module ErrorMessage = {
  type content = {
    errMsg_content: string,
    errMsg_pos: (loc, loc),
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
  block_content: string,
  block_result: blockResult,
  block_pos: (loc, loc),
};
