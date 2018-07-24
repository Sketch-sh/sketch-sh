module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Worker_Types;

  let tryExecute = code => {
    let executeResult = Evaluator.execute(code);
    let stderr = executeResult.stderr;
    let hasSyntaxError =
      switch (stderr) {
      | None => false
      | Some(stderr) =>
        ! (
          stderr
          |> Js.String.indexOf("Syntax error") == (-1)
          && stderr
          |>
          Js.String.indexOf(
            "This comment contains an unterminated string literal",
          ) == (-1)
          && stderr
          |> Js.String.indexOf("String literal begins here") == (-1)
        )
      };
    (hasSyntaxError, executeResult);
  };

  type parseCommand' = {
    startPos: int,
    shouldSkip: bool,
    result: list(wholeProgramExecuteResult),
    lineNum: int,
    lineStart: int,
  };

  let parseCommand = (~f, code) => {
    let length = String.length(code);

    let rec loop = (i, state) => {
      let {startPos, shouldSkip, result, lineNum, lineStart} = state;

      if (i < length) {
        switch (code.[i]) {
        /* Skip spaces between directives */
        | ' ' =>
          shouldSkip ?
            loop(i + 1, {...state, startPos: startPos + 1}) :
            loop(i + 1, state)
        | '\n' =>
          shouldSkip ?
            loop(
              i + 1,
              {
                ...state,
                startPos: startPos + 1,
                lineNum: lineNum + 1,
                lineStart: i + 1,
              },
            ) :
            loop(i + 1, {...state, lineNum: lineNum + 1, lineStart: i + 1})

        | ';' =>
          let buffer =
            Js.String.substring(~from=startPos, ~to_=i + 1, code);
          let (hasSyntaxError, executeResult) = f(buffer);

          hasSyntaxError ?
            loop(i + 1, state) :
            loop(
              i + 1,
              {
                startPos: i + 1,
                shouldSkip: true,
                result: [
                  {buffer, executeResult, pos: (startPos, i + 1)},
                  ...result,
                ],
                lineNum,
                lineStart,
              },
            );
        | _ => loop(i + 1, {...state, shouldSkip: false})
        };
      } else {
        /* Execute the remaining */
        let result =
          if (startPos < i) {
            /*
              TODO:
              - The code may contains actual syntax error
              - check if last buffer is the same at this buffer and get the result
             */
            let buffer =
              Js.String.substring(~from=startPos, ~to_=length + 1, code);
            let (_, executeResult) = f(buffer);
            [
              {buffer, executeResult, pos: (startPos, length + 1)},
              ...result,
            ];
          } else {
            result;
          };
        result |> List.rev;
      };
    };
    loop(
      0,
      {
        startPos: 0,
        shouldSkip: false,
        result: [],
        lineNum: 0,
        lineStart: 0,
      },
    );
  };

  let execute = parseCommand(~f=tryExecute);
};
