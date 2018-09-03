open! Worker_Binding;

[@bs.module] external toplevelPath: string = "../public/toplevel.js";

Worker.importScripts(toplevelPath);

module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);
module Refmt = Worker_Refmt.Make(Worker_BrowserEvaluator);

let obj = {
  "execute": Analyze.execute,
  "executeMany": Analyze.executeMany,
  "link": Analyze.link,
  "linkMany": Analyze.linkMany,
  "refmtMany": Refmt.refmtMany,
};

Comlink.(comlink->(expose(obj, Worker.self)));
