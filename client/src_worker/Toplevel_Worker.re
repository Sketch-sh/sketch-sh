[%%debugger.chrome];
open Toplevel.Types;
open Toplevel.Worker;

[@bs.module]
external import: string = "../public/evaluator/exported-unit.cmis.js";
importScripts(import);
[@bs.module] external import: string = "../public/evaluator/stdlib.cmis.js";
importScripts(import);
[@bs.module] external toplevelPath: string = "../public/evaluator/toplevel.js";
importScripts(toplevelPath);

postMessageFromWorker({w_id: "ready", w_message: Ready});

module Analyze = Worker_Analyze.Make(Worker_BrowserEvaluator);
module Refmt = Worker_Refmt.Make(Worker_BrowserEvaluator);

/* TODO: Fix this hardcoded packages list */
let allowedPackage = [|"owl_base", "base", "containers", "re"|];

let loadedPackages = ref(Belt.Set.String.empty);

let loadSinglePackage = name => {
  let findPackage =
    allowedPackage->Utils.arrayFind(package => package == name);
  switch (findPackage) {
  | None => ()
  | Some(name) =>
    Js.log("Loading package" ++ name);
    importScripts(
      "http://libraries.sketch.sh/" ++ name ++ ".loader.sketch.js",
    );
    loadedPackages := (loadedPackages^)->Belt.Set.String.add(name);
  };
};

let loadPackages = packages =>
  Belt.Set.String.diff(packages, loadedPackages^)
  ->Belt.Set.String.forEach(loadSinglePackage);

self
->onMessageFromTop(event => {
    let {t_id, t_message} = event##data;
    let result =
      switch (t_message) {
      | Execute(lang, blocks, links, packages) =>
        loadPackages(packages);
        ExecuteResult(
          Belt.Result.Ok(Analyze.executeMany(. lang, blocks, links)),
        );
      | Refmt(lang, blocks) =>
        RefmtResult(Belt.Result.Ok(Refmt.refmtMany(lang, blocks)))
      | LoadPackage(packages) =>
        loadPackages(packages);
        LoadPackageResult(Belt.Result.Ok());
      };
    postMessageFromWorker({w_id: t_id, w_message: result});
  });
