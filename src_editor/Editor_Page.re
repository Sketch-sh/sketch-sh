open Worker_Types;

module CodeBlock = {
  [@bs.module "./CodeBlock.js"]
  external reactClass : ReasonReact.reactClass = "default";

  type lineWidgetType =
    | Lw_Error
    | Lw_Value
    | Lw_Stdout;

  [@bs.deriving abstract]
  type lineWidget = {
    [@bs.as "type"]
    typ: lineWidgetType,
    content: string,
    line: int,
  };

  [@bs.deriving abstract]
  type jsProps = {
    [@bs.optional]
    firstLineNumber: int,
    [@bs.optional]
    widgets: array(lineWidget),
    value: string,
    onChange: string => unit,
    onExecute: unit => unit,
  };

  let make =
      (
        ~value,
        ~onChange,
        ~onExecute,
        ~firstLineNumber=?,
        ~widgets=?,
        children,
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props=
        jsProps(
          ~value,
          ~onChange,
          ~onExecute,
          ~firstLineNumber?,
          ~widgets?,
          (),
        ),
      children,
    );
};

open Editor_CodeBlockTypes;

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
                  Editor_Worker.execute(. bc_value)
                  |> then_(
                       result => {
                         let widgets =
                           result
                           |. Belt.List.reduceU(
                                [||],
                                (. acc, exeResult) => {
                                  let {buffer: _, executeResult, pos} = exeResult;
                                  let (_, {line}) = pos;

                                  let evaluate =
                                    executeResult.evaluate
                                    |. Belt.Option.map(content =>
                                         Widget.Lw_Value({line, content})
                                       );

                                  let stdout =
                                    executeResult.stdout
                                    |. Belt.Option.map(content =>
                                         Widget.Lw_Stdout({line, content})
                                       );

                                  let stderr =
                                    executeResult.stderr
                                    |. Belt.Option.map(content =>
                                         Widget.Lw_Error({
                                           line,
                                           content:
                                             content
                                             ++ "\nDebug information: "
                                             ++ pos_of_string(pos),
                                         })
                                       );

                                  let finalWidgets =
                                    [stdout, evaluate, stderr]
                                    |. Belt.List.reduceU(
                                         [], (. acc2, lineWidget) =>
                                         switch (lineWidget) {
                                         | None => acc2
                                         | Some(lw) => [lw, ...acc2]
                                         }
                                       );
                                  Belt.Array.concat(
                                    acc,
                                    finalWidgets |. Belt.List.toArray,
                                  );
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
               | B_Text(_content) => failwith("not implemented")
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
             | B_Text(_) => failwith("not implemented")
             }
           )
        |. ReasonReact.array
      )
    </div>,
};
