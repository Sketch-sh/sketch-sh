type executeResult =
  | Error(string)
  | Ok(string)
  | OkWithLog(string, string);

let execute: string => executeResult;

/* [@bs.val] external reset : unit => unit = "evaluator.reset";
   [@bs.val] external reasonSyntax : unit => unit = "evaluator.reasonSyntax";
   [@bs.val] external mlSyntax : unit => unit = "evaluator.mlSyntax"; */

let parseError: (~content: string, ~error: string) => string;
