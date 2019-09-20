module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Toplevel.Types;

  let refmtMany = (refmtType, codeBlocks) => {
    open Belt.Result;
    let (parser, printer) =
      switch (refmtType) {
      | ReToMl => (Evaluator.parseRE, Evaluator.printML)
      | MlToRe => (Evaluator.parseML, Evaluator.printRE)
      | PrettyPrintRe => (Evaluator.parseRE, Evaluator.printRE)
      };
    let transformer = code =>
      switch (parser(code)) {
      | Ok(ast) => Ok(printer(ast) |> Js.String.trim)
      | Error(error) => Error(error)
      };

    let rec loop = (blocks, acc, hasError) =>
      switch (blocks) {
      | [] => (acc, hasError)
      | [{binput_id: id, binput_value: code}, ...rest] =>
        let result = transformer(code);
        let refmtResult = {refmt_id: id, refmt_value: result};

        let hasError =
          hasError
            ? hasError
            : (
              switch (result) {
              | Ok(_) => false
              | Error(_) => true
              }
            );
        loop(rest, [refmtResult, ...acc], hasError);
      };
    let (result, hasError) = loop(codeBlocks, [], false);
    {result, hasError};
  };
};
