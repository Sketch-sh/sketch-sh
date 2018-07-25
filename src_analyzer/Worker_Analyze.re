/* open Utils; */

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
  };

  /*
    /* Normal */
    let offset = 25;
    let lineStartOffsets =
       [0, 10, 20, 30];
                   ^^ 30 > 25, index[30] = 4 ==> index = 3

    /* Last line */
    let offset = 40;
    let lineStartOffsets =
       [0, 10, 20, 30];
                       ^^ find = -1 => last line, index = 4;
   */
  let toLoc = (lineStartOffsets, offset) => {
    let find =
      lineStartOffsets |> Js.Array.findIndex(value => value >= offset);

    let index =
      switch (find) {
      | 0 => 0
      | (-1) => Js.Array.length(lineStartOffsets) - 1
      | index => index - 1
      };
    let lineStart = lineStartOffsets[index];
    {line: index, col: offset - lineStart, offset};
  };
  let parseCommand = (~f, code) => {
    let length = String.length(code);

    let lineStartOffsets = [|0|];
    let toLoc = toLoc(lineStartOffsets);

    let rec loop = (i, state) => {
      let {startPos, shouldSkip, result} = state;

      if (i < length) {
        switch (code.[i]) {
        /* Skip spaces between directives */
        | ' ' =>
          shouldSkip ?
            loop(i + 1, {...state, startPos: startPos + 1}) :
            loop(i + 1, state)
        | '\n' =>
          lineStartOffsets |> Js.Array.push(i + 1) |> ignore;
          shouldSkip ?
            loop(i + 1, {...state, startPos: startPos + 1}) :
            loop(i + 1, state);

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
                  {
                    buffer,
                    executeResult,
                    pos: (toLoc(startPos), toLoc(i + 1)),
                  },
                  ...result,
                ],
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
              {
                buffer,
                executeResult,
                pos: (toLoc(startPos), toLoc(length + 1)),
              },
              ...result,
            ];
          } else {
            result;
          };
        result |> List.rev;
      };
    };
    loop(0, {startPos: 0, shouldSkip: true, result: []});
  };

  let execute = (. code) => parseCommand(~f=tryExecute, code);
};
