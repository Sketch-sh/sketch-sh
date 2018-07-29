let renderErrorIndicator = (colStart, colEnd, content) =>
  String.make(colStart, ' ')
  ++ String.make(colEnd - colStart, '^')
  ++ "\n"
  ++ content;

let executeRessultToWidget = (result: list(Worker_Types.blockData)) => {
  open Worker_Types;
  open Editor_CodeBlockTypes;

  let widgets =
    result
    |. Belt.List.reduceU(
         [||],
         (. acc, exeResult) => {
           let {block_content: _, block_result: result, block_pos} = exeResult;
           let (_, {line}) = block_pos;

           let evaluate =
             result.blockResult_evaluate
             |. Belt.Option.map(content => Widget.Lw_Value({line, content}));

           let stdout =
             result.blockResult_stdout
             |. Belt.Option.map(content =>
                  Widget.Lw_Stdout({line, content})
                );

           let stderr =
             switch (result.blockResult_stderr) {
             | None => [||]
             | Some(errors) =>
               errors
               |. Belt.Array.mapU(
                    (. error) => {
                      let toWidgetContent = (content: ErrorMessage.content) => {
                        let (
                          {line, col: colStart},
                          {line: _, col: colEnd},
                        ) =
                          content.errMsg_pos;

                        {
                          Editor_CodeBlockTypes.Widget.line,
                          content:
                            renderErrorIndicator(
                              colStart,
                              colEnd,
                              content.errMsg_content,
                            ),
                        };
                      };

                      switch (error) {
                      | ErrorMessage.Err_Warning(content) =>
                        Widget.Lw_Warning(toWidgetContent(content))
                      | Err_Error(content) =>
                        Widget.Lw_Error(toWidgetContent(content))
                      | Err_Unknown(content) =>
                        Widget.Lw_Error({line, content})
                      };
                    },
                  )
             };

           let finalWidgets =
             [|stdout, evaluate|]
             |. Belt.Array.reduceU([||], (. acc2, lineWidget) =>
                  switch (lineWidget) {
                  | None => acc2
                  | Some(lw) => Belt.Array.concat(acc2, [|lw|])
                  }
                );

           Belt.Array.concatMany([|acc, stderr, finalWidgets|]);
         },
       );
  widgets;
};
