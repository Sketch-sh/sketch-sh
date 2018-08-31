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

  let executeMany:
    (. Editor_Types.lang, list((string, string))) =>
    list((string, list(blockData))) =
    (. lang, codeBlocks) => {
      /* Reset before evaluating several blocks */
      Evaluator.reset();

      switch (lang) {
      | Editor_Types.ML => Evaluator.mlSyntax()
      | Editor_Types.RE => Evaluator.reSyntax()
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
};
