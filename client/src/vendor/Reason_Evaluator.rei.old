/* type executeResult =
  | Error(string)
  | Ok(string)
  | OkWithLog(string, string)
  /* type a = | Ok | Result;
     let print =
     fun
     | Ok | Result => "valid"
     | _ => "not valid";
     */
  | OkWithError(string, string)
  /* #help; */
  | Log(string)
  /* #print_length 10 */
  | Nothing; */

type executeResult = {
  evaluate: option(string),
  stderr: option(string),
  stdout: option(string),
};

let execute: string => executeResult;

let reset: unit => unit;
let reasonSyntax : unit => unit;
let mlSyntax : unit => unit;

let parseError: (~content: string, ~error: string) => string;

