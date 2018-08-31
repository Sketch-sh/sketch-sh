open Worker_Binding;

[@bs.module] external toplevelPath: string = "../public/toplevel.js";

Worker.importScripts(toplevelPath);

module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);
module Evaluator = Worker_Evaluator.Make(Worker_BrowserEvaluator);

let obj = {
  "execute": Analyze.execute,
  "executeMany": Analyze.executeMany,
  "refmtMany": Analyze.refmtMany,
};

Comlink.(comlink->(expose(obj, Worker.self)));
