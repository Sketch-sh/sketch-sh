[%%debugger.chrome];
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
  didMount: self => {
    blocks
    |. Belt.Array.forEachWithIndexU((. blockIndex, _block) =>
         self.send(ExecuteBlock(blockIndex))
       );
    ();
  },
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
                           Editor_Page_Utils.executeRessultToWidget(result);
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
