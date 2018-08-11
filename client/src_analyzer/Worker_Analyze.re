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
        !(
          stderr
          |> Js.String.indexOf("Syntax error") == (-1)
          && stderr
          |> Js.String.indexOf("Comment not terminated") == (-1)
          && stderr
          |> Js.String.indexOf("String literal not terminated") == (-1)
          && stderr
          |> Js.String.indexOf(
               "This comment contains an unterminated string literal",
             )
          == (-1)
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
     This function try to execute whenever it encounters a semicolon
     If there is any syntax error, skip to the next semicolon

     It also build up a sorted range array lineStartOffSets for converting offset to {line,column} (0-origin)
   */
  let parseCommand = (~f, code) => {
    open Worker_Location_Utils;

    let length = String.length(code);

    let rec loop = (i, state, lineStartOffsets) => {
      let {startPos, shouldSkip, result} = state;

      if (i < length) {
        switch (code.[i]) {
        /* Skip spaces between directives */
        | ' ' =>
          shouldSkip ?
            loop(
              i + 1,
              {...state, startPos: startPos + 1},
              lineStartOffsets,
            ) :
            loop(i + 1, state, lineStartOffsets)
        | '\n' =>
          shouldSkip ?
            loop(
              i + 1,
              {...state, startPos: startPos + 1},
              Belt.Array.concat(lineStartOffsets, [|i + 1|]),
            ) :
            loop(
              i + 1,
              state,
              Belt.Array.concat(lineStartOffsets, [|i + 1|]),
            )
        | ';' =>
          let buffer = Js.String.substring(~from=startPos, ~to_=i + 1, code);
          let (hasSyntaxError, executeResult) = f(buffer);

          hasSyntaxError ?
            loop(i + 1, state, lineStartOffsets) :
            loop(
              i + 1,
              {
                startPos: i + 1,
                shouldSkip: true,
                result: [
                  {
                    buffer,
                    executeResult,
                    pos: (
                      toLoc(lineStartOffsets, startPos),
                      toLoc(lineStartOffsets, i + 1),
                    ),
                  },
                  ...result,
                ],
              },
              lineStartOffsets,
            );
        | _ => loop(i + 1, {...state, shouldSkip: false}, lineStartOffsets)
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
                pos: (
                  toLoc(lineStartOffsets, startPos),
                  toLoc(lineStartOffsets, length + 1),
                ),
              },
              ...result,
            ];
          } else {
            result;
          };
        result |> List.rev;
      };
    };
    loop(0, {startPos: 0, shouldSkip: true, result: []}, [|0|]);
  };

  let parseAndCorrectStderrPos = (stderr, blockPos) =>
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
      );
  let execute: (. bool, string) => list(blockData) =
    (. reset, code) => {
      if (reset) {
        Evaluator.reset();
      };
      /* Execute and split program into chunks */
      let result = parseCommand(~f=tryExecute, code);
      /* Parse and correct stderr error location */
      let result =
        result
        ->(
            Belt.List.map(blockData => {
              let executeResult = blockData.executeResult;

              let stderr =
                executeResult.stderr
                ->(parseAndCorrectStderrPos(blockData.pos));
              {
                block_content: blockData.buffer,
                block_result: {
                  blockResult_evaluate: executeResult.evaluate,
                  blockResult_stdout: executeResult.stdout,
                  blockResult_stderr: stderr,
                },
                block_pos: blockData.pos,
              };
            })
          );
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
