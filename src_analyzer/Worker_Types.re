type singleExecuteResult = {
  evaluate: option(string),
  stderr: option(string),
  stdout: option(string),
};

type wholeProgramExecuteResult = {
  buffer: string,
  executeResult: singleExecuteResult,
  pos: (int, int),
};
