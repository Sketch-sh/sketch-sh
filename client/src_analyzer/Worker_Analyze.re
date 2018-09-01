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

  let execute: (. bool, string) => (list(blockData), bool) =
    (. reset, code) => {
      if (reset) {
        Evaluator.reset();
      };
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

  open Editor_Types;

  let executeMany:
    (. lang, list((id, string))) => list((id, list(blockData))) =
    (. lang, codeBlocks) => {
      /* Reset before evaluating several blocks */
      Evaluator.reset();

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
        | [(id, code), ...rest] =>
          let (result, hasError) = execute(. false, code);

          hasError ?
            [(id, result), ...acc] : loop(rest, [(id, result), ...acc]);
        };
      loop(codeBlocks, []);
    };

  let logRefmtError = (code, error, targetLang) => {
    let fromLang = ref("RE");
    let toLang = ref("ML");

    switch (targetLang) {
    | ML => ()
    | RE =>
      fromLang := "ML";
      toLang := "RE";
    };

    let message = {j|There was a problem reformating your $fromLang code to $toLang|j};
    /* Notify.error(message ++ ". Check the console for details."); */
    Js.log(message ++ "\n");
    Js.log2("Code:\n", code);
    Js.log2("Error:\n", error);
  };

  let refmtAsLanguage = (code, targetLang) =>
    switch (targetLang) {
    | RE =>
      Belt.Result.(
        switch (Evaluator.parseML(code)) {
        | Ok(ast) => Ok(Evaluator.printRE(ast))
        | Error(error) => Error(error)
        }
      )
    | ML =>
      Belt.Result.(
        switch (Evaluator.parseRE(code)) {
        | Ok(ast) => Ok(Evaluator.printML(ast))
        | Error(error) => Error(error)
        }
      )
    };

  let prettyPrintRe = code =>
    Belt.Result.(
      switch (Evaluator.parseRE(code)) {
      | Ok(ast) => Ok(Evaluator.printRE(ast))
      | Error(error) => Error(error)
      }
    );

  let refmtMany: (. lang, list((id, string)), bool) => list((id, string)) =
    (. targetLang, codeBlocks, prettyPrint) =>
      codeBlocks
      ->Belt.List.mapU(
          (. (blockId, code)) => {
            let result =
              prettyPrint ?
                prettyPrintRe(code) : refmtAsLanguage(code, targetLang);
            switch (result) {
            | Ok(newCode) => (blockId, newCode)
            | Error(error) =>
              logRefmtError(code, error, targetLang);
              (blockId, code);
            };
          },
        );
};
