module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Worker_Types;

  /* let parseAndCorrectStderrPos = (stderr, blockPos) =>
     stderr
     ->(
         Belt.Option.map(stderr =>
           stderr
           ->Worker_ParseLocation.parse
           /* TODO: Investigating this
                This is a hack.
                Toplevel won't stop evaluating if it encounters an error
                This will simply remove all warnings/errors after enountering
                an error
              */
           ->(
               Belt.Array.reduce(
                 ([], false),
                 ((acc, hasError), error: CompilerErrorMessage.t) =>
                 if (hasError) {
                   (acc, hasError);
                 } else {
                   switch (error) {
                   | Err_Unknown(string) => (
                       [CompilerErrorMessage.Err_Unknown(string), ...acc],
                       false,
                     )
                   | Err_Warning(content) => (
                       [Err_Warning(content), ...acc],
                       false,
                     )
                   | Err_Error(content) => (
                       [Err_Error(content), ...acc],
                       true,
                     )
                   };
                 }
               )
             )
           /* TODO:
              Return hasError to stop executing the next code block
              */
           ->Utils.pluckAcc
           ->Belt.List.toArray
           ->(
               Belt.Array.map((error: CompilerErrorMessage.t) =>
                 switch (error) {
                 | Err_Unknown(string) => ErrorMessage.Err_Unknown(string)
                 | Err_Warning(content) =>
                   Err_Warning(
                     Worker_Location_Utils.(
                       compilerErrorMessageToAbsolutePos(content, blockPos)
                       ->compilerErrorMessageToErrorMessage
                     ),
                   )
                 | Err_Error(content) =>
                   Err_Error(
                     Worker_Location_Utils.(
                       compilerErrorMessageToAbsolutePos(content, blockPos)
                       ->compilerErrorMessageToErrorMessage
                     ),
                   )
                 }
               )
             )
         )
       ); */
  let execute: (. bool, string) => list(blockData) =
    (. reset, code) => {
      if (reset) {
        Evaluator.reset();
      };
      let result = Evaluator.execute(code);
      /* Parse and correct stderr error location */
      let result =
        result
        ->(
            Belt.Array.mapU((. js_phraseResult) => {
              open Belt.Result;

              let kind = js_phraseResult->Evaluator.kindGet;
              /* switch (kind) {
                 | "Ok" => Ok(js_phraseResult->Evaluator.valueGet)
                 | "Error" => Error(js_phraseResult->Evaluator.valueGet)
                 | _ =>
                   raise(Invalid_argument("Unknown phrase result " ++ kind))
                 }; */
              /* Currently don't care about whether an error occured */
              switch (kind) {
              | "Ok" => js_phraseResult->Evaluator.valueGet
              | "Error" => js_phraseResult->Evaluator.valueGet
              | _ =>
                raise(Invalid_argument("Unknown phrase result " ++ kind))
              };
            })
          )
        ->(
            Belt.Array.map(phraseContent => {
              let emptyStringToOption =
                fun
                | "" => None
                | str => Some(str);

              let stderr = phraseContent->Evaluator.PhraseContent.stderrGet;

              {
                block_result: {
                  blockResult_evaluate:
                    phraseContent
                    ->Evaluator.PhraseContent.valueGet
                    ->emptyStringToOption,
                  blockResult_stdout:
                    phraseContent
                    ->Evaluator.PhraseContent.stdoutGet
                    ->emptyStringToOption,
                  blockResult_stderr: None,
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
            })
          )
        ->Belt.List.fromArray;
      result;
    };

  let executeMany:
    (. list((string, string))) => list((string, list(blockData))) =
    (. codeBlocks) => {
      /* Reset before evaluating several blocks */
      Evaluator.reset();
      codeBlocks
      ->(Belt.List.mapU((. (id, code)) => (id, execute(. false, code))));
    };
};
