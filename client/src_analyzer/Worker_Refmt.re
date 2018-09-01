module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Editor_Types;

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
            | Ok(newCode) => (blockId, Js.String.trim(newCode))
            | Error(error) =>
              logRefmtError(code, error, targetLang);
              (blockId, code);
            };
          },
        );
};
