let renderErrorIndicator = (colStart, colEnd, content) =>
  String.make(colStart, ' ')
  ++ String.make(
       switch (colEnd - colStart) {
       | 0 => 1
       | a => a
       },
       '^',
     )
  ++ "\n"
  ++ content;

let executeResultToWidget = (result: list(Worker_Types.blockData)) => {
  open Worker_Types;
  open Editor_Types;

  let widgets =
    result
    ->(
        Belt.List.reduceU(
          [||],
          (. acc, exeResult) => {
            let {block_result: result, block_loc} = exeResult;
            let (_, {line}) = block_loc;

            let evaluate =
              result.blockResult_evaluate
              ->(
                  Belt.Option.map(content =>
                    Widget.{lw_line: line, lw_data: Lw_Value(content)}
                  )
                );

            let stdout =
              result.blockResult_stdout
              ->(
                  Belt.Option.map(content =>
                    Widget.{lw_line: line, lw_data: Lw_Stdout(content)}
                  )
                );

            let stderr =
              switch (result.blockResult_stderr) {
              | None => [||]
              | Some(errors) =>
                errors
                ->(
                    Belt.Array.mapU((. error) => {
                      let toWidgetContent = (content: ErrorMessage.content) => {
                        let ({line, col: colStart}, {col: colEnd}) =
                          content.errMsg_loc;

                        (
                          line,
                          renderErrorIndicator(
                            colStart,
                            colEnd,
                            content.errMsg_content,
                          ),
                        );
                      };

                      switch (error) {
                      | ErrorMessage.Err_Warning(content) =>
                        let (lw_line, content) = toWidgetContent(content);
                        Widget.{
                          lw_line,
                          lw_data: Widget.Lw_Warning(content),
                        };
                      | Err_Error(content) =>
                        let (lw_line, content) = toWidgetContent(content);

                        Widget.{lw_line, lw_data: Widget.Lw_Error(content)};
                      | Err_Unknown(content) =>
                        Widget.{
                          lw_line: line,
                          lw_data: Widget.Lw_Error(content),
                        }
                      };
                    })
                  )
              };

            let finalWidgets =
              [|stdout, evaluate|]
              ->(
                  Belt.Array.reduceU([||], (. acc2, lineWidget) =>
                    switch (lineWidget) {
                    | None => acc2
                    | Some(lw) => Belt.Array.concat(acc2, [|lw|])
                    }
                  )
                );

            Belt.Array.concatMany([|acc, stderr, finalWidgets|]);
          },
        )
      );
  widgets;
};

open Editor_Types.Block;
let syncLineNumber: array(block) => array(block) =
  blocks =>
    blocks
    ->(
        Belt.Array.reduceU(
          ([||], 1),
          (. (acc, firstLineNumber), block) => {
            let {b_id, b_data} = block;
            switch (b_data) {
            | B_Code(bcode) =>
              let {bc_value} = bcode;
              let newBCode =
                B_Code({...bcode, bc_firstLineNumber: firstLineNumber});
              (
                Belt.Array.concat(acc, [|{b_id, b_data: newBCode}|]),
                firstLineNumber + bc_value->Utils.js_countLine,
              );
            | B_Text(_) => (
                Belt.Array.concat(acc, [|block|]),
                firstLineNumber,
              )
            };
          },
        )
      )
    ->Utils.pluckAcc;

let emptyCodeBlock = () =>
  B_Code({bc_value: "", bc_firstLineNumber: 1, bc_widgets: [||]});

let emptyTextBlock = () => B_Text("");

let findLastCodeBlock = blocks => {
  let length = Array.length(blocks);
  let rec loop = i =>
    if (i >= 0) {
      switch (blocks[i]) {
      | {b_id, b_data: B_Code(_)} => Some(b_id)
      | _ => loop(i - 1)
      };
    } else {
      None;
    };
  loop(length - 1);
};

let getFirstLineFromDiff = (diff: CodeMirror.EditorChange.t) => {
  let fromPos = diff->CodeMirror.EditorChange.fromGet;
  let line = fromPos->CodeMirror.Position.lineGet;

  line;
};
