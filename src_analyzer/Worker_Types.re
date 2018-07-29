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

type final_evaluationResult = {
  fn_evaluate: option(string),
  fn_stderr: option(array(CompilerErrorMessage.t)),
  fn_stdout: option(string),
};

type final_programEvaluationResult = {
  fn_buffer: string,
  fn_result: final_evaluationResult,
  fn_pos: (loc, loc),
};
