module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Worker_Types;

  let emptyStringToOption =
    fun
    | "" => None
    | str => Some(str);

  let parseStderr = stderr =>
    stderr->(Belt.Option.map(stderr => stderr->Worker_ParseLocation.parse));

  let makeBlockResult = (~lastLoc=?, phraseContent) => {
    let stderr = phraseContent->Evaluator.PhraseContent.stderrGet;
    let optLoc =
      phraseContent->Evaluator.PhraseContent.locGet->Js.Nullable.toOption;
    let loc =
      switch (optLoc) {
      | None =>
        /*
         * TODO: Improve location estimation
         * Ptop_dir doesn't have a location
         * So we can estimate the location by
         * Increasing the line number of the last phrase by 1
         */
        switch (lastLoc) {
        | None => ({line: 0, col: 0}, {line: 0, col: 0})
        | Some((_, {line, col})) => (
            {line: line + 1, col: col + 1},
            {line: line + 1, col: col + 1},
          )
        }
      | Some(js_loc) => js_loc->Worker_Location_Utils.compilerLocToLoc
      };

    let result = {
      block_result: {
        blockResult_evaluate:
          phraseContent->Evaluator.PhraseContent.valueGet->emptyStringToOption,
        blockResult_stdout:
          phraseContent
          ->Evaluator.PhraseContent.stdoutGet
          ->emptyStringToOption,
        blockResult_stderr: stderr->emptyStringToOption->parseStderr,
      },
      block_loc: loc,
    };
    (loc, result);
  };

  open Editor_Types;

  let execute: (. string) => (list(blockData), bool) =
    (. code) => {
      let result = Evaluator.execute(code);
      let length = Belt.Array.length(result);

      let rec loop = (i, acc, lastLoc) =>
        if (i < length) {
          let js_phraseResult = result[i];

          switch (js_phraseResult->Evaluator.kindGet) {
          | "Ok" =>
            let (loc, result) =
              js_phraseResult->Evaluator.valueGet
              |> makeBlockResult(~lastLoc?);
            loop(i + 1, [result, ...acc], Some(loc));
          | "Error" =>
            let (_loc, result) =
              js_phraseResult->Evaluator.valueGet
              |> makeBlockResult(~lastLoc?);
            ([result, ...acc], true);
          | kind => raise(Invalid_argument("Unknown phrase result " ++ kind))
          };
        } else {
          (acc->Belt.List.reverse, false);
        };
      loop(0, [], None);
    };

  let link: (. lang, string, string) => linkResult =
    (. lang, name, code) => {
      open Evaluator;

      switch (lang) {
      | ML => mlSyntax()
      | RE => reSyntax()
      };
      let lang = lang->langToString->String.lowercase;
      let js_linkResult = insertModule(. name, code, lang);
      Belt.Result.(
        switch (js_linkResult->LinkResult.kindGet) {
        | "Ok" => Ok()
        | "Error" => Error(js_linkResult->LinkResult.valueGet)
        | _kind => Error("unknown link result")
        }
      );
    };

  exception Not_Implemented;

  let linkMany: (. list(Link.link)) => list(Toplevel.Types.linkResult) =
    (. links) => {
      open Link;

      let rec loop = (links, acc) =>
        switch (links) {
        | [] => acc
        | [singleLink, ...rest] =>
          let (link, result) =
            switch (singleLink) {
            | Internal(internalLink) =>
              let {lang, name, code} = internalLink;
              let result = link(. lang, name, code);
              (singleLink, result);
            | External(_) => raise(Not_Implemented)
            };

          let linkResult: Toplevel.Types.linkResult = {link, result};

          loop(rest, [linkResult, ...acc]);
        };

      loop(links, []);
    };

  let executeMany:
    (
      . Editor_Types.lang,
      list(Toplevel.Types.blockInput),
      list(Link.link)
    ) =>
    (list(Toplevel.Types.linkResult), list(Toplevel.Types.blockResult)) =
    (. lang, codeBlocks, links) => {
      Evaluator.reset();

      let linkResults = linkMany(. links);
      open Toplevel.Types;

      switch (lang) {
      | ML => Evaluator.mlSyntax()
      | RE => Evaluator.reSyntax()
      };
      /*
       * Execute blocks in order
       * Stop when encountering error in a block
       */
      let rec loop = (blocks, acc) =>
        switch (blocks) {
        | [] => acc
        | [{binput_id: id, binput_value: code}, ...rest] =>
          let (result, hasError) = execute(. code);

          let currentBlockResult = {id, result};
          hasError
            ? [currentBlockResult, ...acc]
            : loop(rest, [currentBlockResult, ...acc]);
        };
      let executeResults = loop(codeBlocks, []);

      (linkResults, executeResults);
    };
  let executeEmbed:
    (. Editor_Types.lang, string) => list(Worker_Types.blockData) =
    (. lang, code) => {
      Evaluator.reset();
      switch (lang) {
      | ML => Evaluator.mlSyntax()
      | RE => Evaluator.reSyntax()
      };

      let (result, _hasError) = execute(. code);
      result;
    };
};
