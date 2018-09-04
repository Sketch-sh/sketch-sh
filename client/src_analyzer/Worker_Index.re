/* open! Worker_Binding;

   [@bs.module] external toplevelPath: string = "../public/toplevel.js";

   Worker.importScripts(toplevelPath);

   module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);
   module Refmt = Worker_Refmt.Make(Worker_BrowserEvaluator);

   let obj = {
     "execute": Analyze.execute,
     "executeMany": Analyze.executeMany,
     "refmtMany": Refmt.refmtMany,
     /* Refmt functions */
     "reToMl": code =>
       Belt.Result.(
         switch (Evaluator.parseRE(code)) {
         | Ok(ast) => Ok(Evaluator.printML(ast))
         | Error(error) => Error(error)
         }
       ),
     "mlToRe": code =>
       Belt.Result.(
         switch (Evaluator.parseML(code)) {
         | Ok(ast) => Ok(Evaluator.printRE(ast))
         | Error(error) => Error(error)
         }
       ),
     "reToRe": code =>
       Belt.Result.(
         switch (Evaluator.parseRE(code)) {
         | Ok(ast) => Ok(Evaluator.printRE(ast))
         | Error(error) => Error(error)
         }
       ),
     "link": Analyze.link,
     "linkMany": Analyze.linkMany,
   };

   Comlink.(comlink->(expose(obj, Worker.self))); */
