[@bs.scope "evaluator"] [@bs.module "../public/reason.js"]
external js_execute : string => Worker_Evaluator.js_executeResult = "execute";
let js_execute = js_execute;

[@bs.scope "evaluator"] [@bs.module "../public/reason.js"]
external js_reset : unit => unit = "reset";

let js_reset = js_reset;
