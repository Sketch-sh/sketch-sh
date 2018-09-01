module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Editor_Types;

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

  open Evaluator.Refmt;
  let refmtMany: (. lang, list((id, string)), bool) => refmtManyResult =
    (. targetLang, codeBlocks, prettyPrint) => {
      let rec loop = (i, acc) =>
        if (i < List.length(codeBlocks)) {
          let (blockId, code) = codeBlocks->List.nth(i);
          let result =
            prettyPrint ?
              prettyPrintRe(code) : refmtAsLanguage(code, targetLang);
          switch (result) {
          | Ok(newCode) =>
            loop(i + 1, [(blockId, Js.String.trim(newCode), None), ...acc])
          | Error(error) =>
            loop(i + 1, [(blockId, code, Some(error)), ...acc])
          };
        } else {
          acc;
        };
      loop(0, []);
    };
};
