[%%debugger.chrome];
open Utils;
open Editor_CodeBlockTypes;

type bcode = {
  bc_value: string,
  bc_firstLineNumber: int,
  bc_lines: int,
  bc_widgets: array(Widget.t),
};

type id = string;
type blockType =
  | B_Code(bcode)
  | B_Text(string);

type block = {
  b_id: id,
  b_data: blockType,
};

type state = {blocks: array(block)};

type action =
  | UpdateBlockValue(id, string)
  | ExecuteBlock(id)
  | Block_AddWidgets(id, array(Widget.t));

let component = ReasonReact.reducerComponent("Editor_Page");

let make = (~blocks: array(block), _children) => {
  ...component,
  initialState: () => {blocks: blocks},
  didMount: self => {
    blocks
    |. Belt.Array.forEachU((. {b_id}) => self.send(ExecuteBlock(b_id)));
    ();
  },
  reducer: (action, state) =>
    switch (action) {
    | Block_AddWidgets(blockId, widgets) =>
      ReasonReact.Update({
        /* ...state, */
        blocks:
          state.blocks
          |. Belt.Array.mapU(
               (. block) => {
                 let {b_id, b_data} = block;
                 if (b_id != blockId) {
                   block;
                 } else {
                   switch (b_data) {
                   | B_Text(_) => block
                   | B_Code(bcode) => {
                       b_id,
                       b_data: B_Code({...bcode, bc_widgets: widgets}),
                     }
                   };
                 };
               },
             ),
      })
    | ExecuteBlock(blockId) =>
      let block = state.blocks |. arrayFind(({b_id}) => b_id == blockId);
      switch (block) {
      | None => ReasonReact.NoUpdate
      | Some(block) =>
        switch (block.b_data) {
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
                           self.send(Block_AddWidgets(blockId, widgets)),
                         );
                       },
                     )
                  |> catch(error => resolve(Js.log(error)))
                  |> ignore
                )
            ),
          )
        }
      };

    | UpdateBlockValue(blockId, newValue) =>
      ReasonReact.Update({
        /* ...state, */
        blocks: {
          let nextFirstLineNumber = ref(None);
          state.blocks
          |. Belt.Array.mapU(
               (. block) => {
                 let {b_id, b_data} = block;
                 switch (b_data) {
                 | B_Code(bcode) =>
                   switch (nextFirstLineNumber^) {
                   | None =>
                     if (b_id != blockId) {
                       block;
                     } else {
                       let newValueLines =
                         newValue
                         |> Js.String.split("\n")
                         |> Js.Array.length;
                       (
                         if (bcode.bc_lines != newValueLines) {
                           nextFirstLineNumber := Some(newValueLines + 1);
                         }
                       )
                       |> ignore;
                       {
                         b_id,
                         b_data:
                           B_Code({
                             ...bcode,
                             bc_value: newValue,
                             bc_lines: newValueLines,
                           }),
                       };
                     }
                   | Some(lineNum) =>
                     nextFirstLineNumber :=
                       Some(lineNum + bcode.bc_lines + 1);
                     {
                       b_id,
                       b_data:
                         B_Code({...bcode, bc_firstLineNumber: lineNum}),
                     };
                   }
                 | B_Text(_) =>
                   if (b_id != blockId) {
                     block;
                   } else {
                     {b_id, b_data: B_Text(newValue)};
                   }
                 };
               },
             );
        },
      })
    },
  render: ({send, state}) =>
    <div className="pageSizer">
      (
        state.blocks
        |. Belt.Array.mapU((. {b_id, b_data}) =>
             <div key=b_id id=b_id className="cell__container">
               (
                 switch (b_data) {
                 | B_Code({bc_value, bc_widgets, bc_firstLineNumber}) =>
                   <div className="source-editor">
                     <Editor_CodeBlock
                       value=bc_value
                       onChange=(
                         newValue => send(UpdateBlockValue(b_id, newValue))
                       )
                       onExecute=(() => send(ExecuteBlock(b_id)))
                       widgets=bc_widgets
                       firstLineNumber=bc_firstLineNumber
                     />
                   </div>
                 | B_Text(text) =>
                   <div className="text-editor">
                     <Editor_TextBlock
                       value=text
                       onChange=(
                         newValue => send(UpdateBlockValue(b_id, newValue))
                       )
                     />
                   </div>
                 }
               )
               <div className="cell__controls">
                 <button> ("Delete cell" |> str) </button>
                 <button> ("Add text cell" |> str) </button>
                 <button> ("Add code cell" |> str) </button>
               </div>
             </div>
           )
        |. ReasonReact.array
      )
    </div>,
};
