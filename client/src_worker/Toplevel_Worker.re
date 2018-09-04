[%%debugger.chrome];
open Toplevel.Types;
open Toplevel.Worker;

[@bs.module] external toplevelPath: string = "../public/toplevel.js";
importScripts(toplevelPath);

postMessageFromWorker({w_id: "ready", w_message: Ready});

module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);
module Refmt = Worker_Refmt.Make(Worker_BrowserEvaluator);
self
->onMessageFromTop(
    event => {
      let {t_id, t_message} = event##data;
      let result =
        switch (t_message) {
        | Link(links) => LinkResult(Analyze.linkMany(. links))
        | Execute(lang, blocks) =>
          ExecuteResult(Belt.Result.Ok(Analyze.executeMany(lang, blocks)))
        | Refmt(lang, blocks) =>
          RefmtResult(Belt.Result.Ok(Refmt.refmtMany(lang, blocks)))
        };
      postMessageFromWorker({w_id: t_id, w_message: result});
    },
  );
