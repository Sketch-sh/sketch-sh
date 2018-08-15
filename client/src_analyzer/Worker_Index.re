open Worker_Binding;

Worker.importScripts("/reason_v2.js");

module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);

let obj = {"execute": Analyze.execute, "executeMany": Analyze.executeMany};

Comlink.(comlink->(expose(obj, Worker.self)));
