[%%debugger.chrome];
open Worker_Types;
open Editor_CodeBlockTypes;
open Utils;

type bcode = {
  bc_value: string,
  bc_firstLineNumber: int,
  bc_lines: int,
  bc_widgets: array(Widget.t),
};

type block =
  | B_Code(bcode)
  | B_Text(string);

type state = {blocks: array(block)};

type action =
  | UpdateBlockValue(int, string)
  | ExecuteBlock(int)
  | Block_AddWidgets(int, array(Widget.t));

let component = ReasonReact.reducerComponent("Editor_Page");

let make = (~blocks: array(block), _children) => {
  ...component,
  initialState: () => {blocks: blocks},
  reducer: (action, state) =>
    switch (action) {
    | Block_AddWidgets(blockIndex, widgets) =>
      ReasonReact.Update({
        /* ...state, */
        blocks:
          state.blocks
          |. Belt.Array.mapWithIndexU((. i, block) =>
               if (i != blockIndex) {
                 block;
               } else {
                 switch (block) {
                 | B_Text(_) => block
                 | B_Code(bcode) => B_Code({...bcode, bc_widgets: widgets})
                 };
               }
             ),
      })
    | ExecuteBlock(blockIndex) =>
      switch (state.blocks |. Belt.Array.get(blockIndex)) {
      | None => ReasonReact.NoUpdate
      | Some(b) =>
        switch (b) {
        | B_Text(_) => ReasonReact.NoUpdate
        | B_Code({bc_value}) =>
          ReasonReact.SideEffects(
            (
              self =>
                Js.Promise.(
                  Editor_Worker.execute(. true, bc_value)
                  |> then_(
                       result => {
                         let widgets =
                           result
                           |. Belt.List.reduceU(
                                [||],
                                (. acc, exeResult) => {
                                  let {
                                    fn_buffer: _,
                                    fn_result: result,
                                    fn_pos,
                                  } = exeResult;
                                  let (_, {line}) = fn_pos;

                                  let evaluate =
                                    result.fn_evaluate
                                    |. Belt.Option.map(content =>
                                         Widget.Lw_Value({line, content})
                                       );

                                  let stdout =
                                    result.fn_stdout
                                    |. Belt.Option.map(content =>
                                         Widget.Lw_Stdout({line, content})
                                       );

                                  let stderr =
                                    switch (result.fn_stderr) {
                                    | None => [||]
                                    | Some(errors) =>
                                      errors
                                      |. Belt.Array.map(
                                           error => {
                                             let toWidgetContent =
                                                 (content: Error.content) => {
                                               let (
                                                 {
                                                   lno_line,
                                                   lno_col: colStart,
                                                 },
                                                 {
                                                   lno_line: _,
                                                   lno_col: colEnd,
                                                 },
                                               ) =
                                                 content.pos;
                                               Js.log3(
                                                 colStart,
                                                 colEnd,
                                                 content.content,
                                               );

                                               {
                                                 Editor_CodeBlockTypes.Widget.line: lno_line,
                                                 content:
                                                   renderErrorIndicator(
                                                     colStart,
                                                     colEnd,
                                                     content.content,
                                                   ),
                                               };
                                             };

                                             switch (error) {
                                             | Err_Warning(content) =>
                                               Js.log("warnng");
                                               Widget.Lw_Warning(
                                                 toWidgetContent(content),
                                               );
                                             | Err_Error(content) =>
                                               Widget.Lw_Error(
                                                 toWidgetContent(content),
                                               )
                                             | Err_Unknown(content) =>
                                               Widget.Lw_Error({
                                                 line,
                                                 content,
                                               })
                                             };
                                           },
                                         )
                                    };

                                  let finalWidgets =
                                    [|stdout, evaluate|]
                                    |. Belt.Array.reduceU(
                                         [||], (. acc2, lineWidget) =>
                                         switch (lineWidget) {
                                         | None => acc2
                                         | Some(lw) =>
                                           Belt.Array.concat(acc2, [|lw|])
                                         }
                                       );

                                  Belt.Array.concatMany([|
                                    acc,
                                    stderr,
                                    finalWidgets,
                                  |]);
                                },
                              );
                         resolve(
                           self.send(
                             Block_AddWidgets(blockIndex, widgets),
                           ),
                         );
                       },
                     )
                  |> catch(error => resolve(Js.log(error)))
                  |> ignore
                )
            ),
          )
        }
      }

    | UpdateBlockValue(blockIndex, newValue) =>
      ReasonReact.Update({
        /* ...state, */
        blocks: {
          let nextFirstLineNumber = ref(None);
          state.blocks
          |. Belt.Array.mapWithIndexU((. i, block) =>
               switch (block) {
               | B_Code(bcode) =>
                 switch (nextFirstLineNumber^) {
                 | None =>
                   if (i != blockIndex) {
                     block;
                   } else {
                     let newValueLines =
                       newValue |> Js.String.split("\n") |> Js.Array.length;
                     (
                       if (bcode.bc_lines != newValueLines) {
                         nextFirstLineNumber := Some(newValueLines + 1);
                       }
                     )
                     |> ignore;
                     B_Code({
                       ...bcode,
                       bc_value: newValue,
                       bc_lines: newValueLines,
                     });
                   }
                 | Some(lineNum) =>
                   nextFirstLineNumber := Some(lineNum + bcode.bc_lines + 1);
                   B_Code({...bcode, bc_firstLineNumber: lineNum});
                 }
               | B_Text(_) =>
                 if (i != blockIndex) {
                   block;
                 } else {
                   B_Text(newValue);
                 }
               }
             );
        },
      })
    },
  render: ({send, state}) =>
    <div className="pageSizer">
      (
        state.blocks
        |. Belt.Array.mapWithIndexU((. index, block) =>
             switch (block) {
             | B_Code({bc_value, bc_widgets, bc_firstLineNumber}) =>
               <div key=(index |> string_of_int) className="cell__container">
                 <div className="source-editor">
                   <Editor_CodeBlock
                     value=bc_value
                     onChange=(
                       newValue => send(UpdateBlockValue(index, newValue))
                     )
                     onExecute=(() => send(ExecuteBlock(index)))
                     widgets=bc_widgets
                     firstLineNumber=bc_firstLineNumber
                   />
                 </div>
               </div>
             | B_Text(text) =>
               <div key=(index |> string_of_int) className="cell__container">
                 <div className="text-editor">
                   <Editor_TextBlock
                     value=text
                     onChange=(
                       newValue => send(UpdateBlockValue(index, newValue))
                     )
                   />
                 </div>
               </div>
             }
           )
        |. ReasonReact.array
      )
    </div>,
};
