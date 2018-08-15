[@bs.scope "evaluator"] [@bs.module "../public/reason_v2.js"]
external js_execute: string => Worker_Evaluator.Types.execResult = "execute";
let js_execute = js_execute;

[@bs.scope "evaluator"] [@bs.module "../public/reason_v2.js"]
external js_reset: unit => unit = "reset";

let js_reset = js_reset;
