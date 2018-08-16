module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Worker_Types;

  let emptyStringToOption =
    fun
    | "" => None
    | str => Some(str);

  let parseStderr = stderr =>
    stderr->(Belt.Option.map(stderr => stderr->Worker_ParseLocation.parse));

  let makeBlockResult = phraseContent => {
    let stderr = phraseContent->Evaluator.PhraseContent.stderrGet;

    {
      block_result: {
        blockResult_evaluate:
          phraseContent->Evaluator.PhraseContent.valueGet->emptyStringToOption,
        blockResult_stdout:
          phraseContent
          ->Evaluator.PhraseContent.stdoutGet
          ->emptyStringToOption,
        blockResult_stderr: stderr->emptyStringToOption->parseStderr,
      },
      block_loc:
        phraseContent
        ->Evaluator.PhraseContent.locGet
        ->Js.Nullable.toOption
        ->{
            let f = optLoc =>
              switch (optLoc) {
              | None => failwith("Unimplemented")
              | Some(js_loc) => js_loc
              };
            f;
          }
        ->Worker_Location_Utils.compilerLocToLoc,
    };
  };

  let execute: (. bool, string) => (list(blockData), bool) =
    (. reset, code) => {
      if (reset) {
        Evaluator.reset();
      };
      let result = Evaluator.execute(code);
      let length = Belt.Array.length(result);

      let rec loop = (i, acc) =>
        if (i < length) {
          let js_phraseResult = result[i];

          switch (js_phraseResult->Evaluator.kindGet) {
          | "Ok" =>
            loop(
              i + 1,
              [js_phraseResult->Evaluator.valueGet->makeBlockResult, ...acc],
            )
          | "Error" => (
              [js_phraseResult->Evaluator.valueGet->makeBlockResult, ...acc],
              true,
            )
          | kind => raise(Invalid_argument("Unknown phrase result " ++ kind))
          };
        } else {
          (acc, false);
        };
      loop(0, []);
    };

  let executeMany:
    (. list((string, string))) => list((string, list(blockData))) =
    (. codeBlocks) => {
      /* Reset before evaluating several blocks */
      Evaluator.reset();

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
