open Utils;

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
    {line: index, col: offset - lineStart - 1, offset};
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
          lineStartOffsets |> Js.Array.push(i) |> ignore;
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

  let parseAndCorrectStderrPos = (stderr, ~lineFrom, ~colFrom) =>
    stderr
    |. Belt.Option.map(stderr =>
         stderr
         |. Worker_ParseLocation.parse
         /* TODO: Investigating this
              This is a hack.
              Toplevel won't stop evaluating if it encounters an error
              This will simply remove all warnings/errors after enountering
              an error
            */
         |. Belt.Array.reduce(
              ([], false), ((acc, hasError), error: Error.t) =>
              if (hasError) {
                (acc, hasError);
              } else {
                switch (error) {
                | Err_Unknown(string) => (
                    [Error.Err_Unknown(string), ...acc],
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
         /* TODO:
            Return hasError to stop executing the next code block
            */
         |. (((acc, hasError)) => acc)
         |. Belt.List.toArray
         |. Belt.Array.map(
              (error: Error.t) => {
                let correctLoc = (content: Error.content) => {
                  let (from, to_) = content.pos;
                  {
                    ...content,
                    pos: (
                      {
                        lno_line: from.lno_line + lineFrom,
                        lno_col: from.lno_col + colFrom,
                      },
                      {
                        lno_line: to_.lno_line + lineFrom,
                        lno_col: to_.lno_col + colFrom,
                      },
                    ),
                  };
                };
                switch (error) {
                | Err_Unknown(string) => Error.Err_Unknown(string)
                | Err_Warning(content) =>
                  Err_Warning(content |. correctLoc)
                | Err_Error(content) => Err_Error(content |. correctLoc)
                };
              },
            )
       );
  let execute =
    (. reset, code) => {
      if (reset) {
        Evaluator.reset();
      };
      /* Execute and split program into chunks */
      let result = parseCommand(~f=tryExecute, code);

      /* Parse and correct stderr error location */
      let result =
        result
        |. Belt.List.map(
             directiveResult => {
               let ({line: lineFrom, col: colFrom}, _) =
                 directiveResult.pos;

               let executeResult = directiveResult.executeResult;
               let stderr =
                 executeResult.stderr
                 |. parseAndCorrectStderrPos(~lineFrom, ~colFrom);
               {
                 fn_buffer: directiveResult.buffer,
                 fn_result: {
                   fn_evaluate: executeResult.evaluate,
                   fn_stdout: executeResult.stdout,
                   fn_stderr: stderr,
                 },
                 fn_pos: directiveResult.pos,
               };
             },
           );
      result;
    };
};
