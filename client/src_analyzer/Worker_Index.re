open Worker_Binding;

Worker.importScripts("/berror.js");
Worker.importScripts("/reason.js");

module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);

let obj = {"execute": Analyze.execute};

Comlink.(comlink |. expose(obj, Worker.self));
