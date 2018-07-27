type locNoOffset = {
  lno_line: int,
  lno_col: int,
};

module Error = {
  type content = {
    content: string,
    pos: (locNoOffset, locNoOffset),
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
  offset: int,
};
let loc_of_string = ({line, col}) => {j|{line: $(line), ch: $(col)}|j};
let pos_of_string = ((from, to_)) =>
  "editor.getRange("
  ++ loc_of_string(from)
  ++ ","
  ++ loc_of_string(to_)
  ++ ")";

type wholeProgramExecuteResult = {
  buffer: string,
  executeResult: singleExecuteResult,
  pos: (loc, loc),
};

type final_evaluationResult = {
  fn_evaluate: option(string),
  fn_stderr: option(array(Error.t)),
  fn_stdout: option(string),
};

type final_programEvaluationResult = {
  fn_buffer: string,
  fn_result: final_evaluationResult,
  fn_pos: (loc, loc),
};
