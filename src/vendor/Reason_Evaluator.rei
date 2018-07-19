type executeResult =
  | Error(string)
  | Ok(string)
  | OkWithLog(string, string);

let execute: string => executeResult;
