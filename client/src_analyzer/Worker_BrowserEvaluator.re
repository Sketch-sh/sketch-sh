[@bs.val]
external js_execute : string => Worker_Evaluator.js_executeResult =
  "evaluator.execute";

let js_execute = js_execute;

[@bs.val] external js_reset : unit => unit = "evaluator.reset";

let js_reset = js_reset;
