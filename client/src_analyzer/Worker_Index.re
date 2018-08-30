open Worker_Binding;

%bs.raw
{|
  importScripts("/libs/exported-unit.cmis.js");
  importScripts("/libs/stdlib.cmis.js");
  importScripts("/libs/toplevel.js");

  importScripts("https://libraries.sketch.sh/owl_base.loader.sketch.js");
|};

module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);
module Evaluator = Worker_Evaluator.Make(Worker_BrowserEvaluator);

let obj = {
  "execute": Analyze.execute,
  "executeMany": Analyze.executeMany,
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
};

Comlink.(comlink->(expose(obj, Worker.self)));
