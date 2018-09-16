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

type linkResult = Belt.Result.t(unit, string);
