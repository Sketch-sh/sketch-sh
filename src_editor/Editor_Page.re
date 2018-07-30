[%%debugger.chrome];
open Utils;
open Editor_CodeBlockTypes;
open Editor_Types.Block;

type state = {
  blocks: array(block),
  focusedBlock: option((id, blockTyp, focusChangeType)),
};

type action =
  | Block_Add(id, blockData)
  | Block_Execute(id)
  | Block_Delete(id)
  | Block_Focus(id, blockTyp)
  | Block_UpdateValue(id, string)
  | Block_AddWidgets(id, array(Widget.t))
  | Block_FocusUp(id)
  | Block_FocusDown(id);

let blockControlsButtons = (b_id, send) =>
  <div className="cell__controls-buttons">
    <button onClick=(_ => send(Block_Delete(b_id)))>
      ("Delete block" |. str)
    </button>
    <button
      onClick=(
        _ => send(Block_Add(b_id, Editor_Page_Utils.emptyTextBlock()))
      )>
      ("Add text block" |. str)
    </button>
    <button
      onClick=(
        _ => send(Block_Add(b_id, Editor_Page_Utils.emptyCodeBlock()))
      )>
      ("Add code block" |. str)
    </button>
  </div>;

let blockHint = (b_id, send) =>
  <div className="cell__controls-hint">
    <button
      onClick=(_ => send(Block_Execute(b_id)))
      className="cell__controls-hint--run">
      ("run" |. str)
    </button>
    <span>
      (" or press " |. str)
      <kdb> ("Shift" |. str) </kdb>
      (" + " |. str)
      <kdb> ("Enter" |. str) </kdb>
    </span>
  </div>;

let component = ReasonReact.reducerComponent("Editor_Page");

let make = (~blocks: array(block), _children) => {
  ...component,
  initialState: () => {blocks, focusedBlock: None},
  didMount: self => {
    blocks
    |. Belt.Array.forEachU((. {b_id}) => self.send(Block_Execute(b_id)));
    ();
  },
  reducer: (action, state) =>
    switch (action) {
    | Block_AddWidgets(blockId, widgets) =>
      ReasonReact.Update({
        ...state,
        blocks:
          state.blocks
          |. Belt.Array.mapU((. block) => {
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
             }),
      })
    | Block_Execute(blockId) =>
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
                  |> then_(result => {
                       let widgets =
                         Editor_Page_Utils.executeRessultToWidget(result);
                       resolve(
                         self.send(Block_AddWidgets(blockId, widgets)),
                       );
                     })
                  |> catch(error => resolve(Js.log(error)))
                  |> ignore
                )
            ),
          )
        }
      };

    | Block_UpdateValue(blockId, newValue) =>
      ReasonReact.Update({
        ...state,
        blocks:
          state.blocks
          |. Belt.Array.mapU((. block) => {
               let {b_id, b_data} = block;
               if (b_id != blockId) {
                 block;
               } else {
                 switch (b_data) {
                 | B_Code(bcode) => {
                     b_id,
                     b_data: B_Code({...bcode, bc_value: newValue}),
                   }
                 | B_Text(_) => {b_id, b_data: B_Text(newValue)}
                 };
               };
             })
          |. Editor_Page_Utils.syncLineNumber,
      })
    | Block_Delete(blockId) =>
      ReasonReact.Update({
        blocks:
          state.blocks
          |. Belt.Array.keepU((. {b_id}) => b_id != blockId)
          |. Editor_Page_Utils.syncLineNumber,
        focusedBlock:
          switch (state.focusedBlock) {
          | None => None
          | Some((focusedBlock, _, _)) =>
            focusedBlock == blockId ? None : state.focusedBlock
          },
      })
    | Block_Focus(blockId, blockTyp) =>
      ReasonReact.Update({
        ...state,
        focusedBlock: Some((blockId, blockTyp, FcTyp_EditorFocus)),
      })
    | Block_Add(afterBlockId, blockType) =>
      ReasonReact.Update({
        ...state,
        blocks:
          state.blocks
          |. Belt.Array.reduceU(
               [||],
               (. acc, block) => {
                 let {b_id} = block;
                 if (b_id != afterBlockId) {
                   Belt.Array.concat(acc, [|block|]);
                 } else {
                   Belt.Array.concat(
                     acc,
                     [|
                       block,
                       {b_id: Utils.generateId(), b_data: blockType},
                     |],
                   );
                 };
               },
             )
          |. Editor_Page_Utils.syncLineNumber,
      })
    | Block_FocusUp(blockId) =>
      let upperBlockId = {
        let rec loop = i =>
          if (i >= 0) {
            let {b_id} = state.blocks[i];
            if (b_id == blockId && i != 0) {
              let {b_id, b_data} = state.blocks[i - 1];
              switch (b_data) {
              | B_Code(_) => Some((b_id, BTyp_Code))
              | B_Text(_) => Some((b_id, BTyp_Text))
              };
            } else {
              loop(i - 1);
            };
          } else {
            None;
          };
        loop((state.blocks |. Belt.Array.length) - 1);
      };
      switch (upperBlockId) {
      | None => ReasonReact.NoUpdate
      | Some((upperBlockId, blockTyp)) =>
        ReasonReact.Update({
          ...state,
          focusedBlock: Some((upperBlockId, blockTyp, FcTyp_BlockFocusUp)),
        })
      };
    | Block_FocusDown(blockId) =>
      let lowerBlockId = {
        let length = state.blocks |. Belt.Array.length;
        let rec loop = i =>
          if (i < length) {
            let {b_id} = state.blocks[i];
            if (b_id == blockId && i != length - 1) {
              let {b_id, b_data} = state.blocks[i + 1];
              switch (b_data) {
              | B_Code(_) => Some((b_id, BTyp_Code))
              | B_Text(_) => Some((b_id, BTyp_Text))
              };
            } else {
              loop(i + 1);
            };
          } else {
            None;
          };
        loop(0);
      };
      switch (lowerBlockId) {
      | None => ReasonReact.NoUpdate
      | Some((lowerBlockId, blockTyp)) =>
        ReasonReact.Update({
          ...state,
          focusedBlock: Some((lowerBlockId, blockTyp, FcTyp_BlockFocusDown)),
        })
      };
    },
  render: ({send, state}) => {
    let lastCodeBlockId = Editor_Page_Utils.findLastCodeBlock(state.blocks);
    <div className="pageSizer">
      (
        state.blocks
        |. Belt.Array.mapU((. {b_id, b_data}) =>
             switch (b_data) {
             | B_Code({bc_value, bc_widgets, bc_firstLineNumber}) =>
               <div key=b_id id=b_id className="cell__container">
                 <div className="source-editor">
                   <Editor_CodeBlock
                     value=bc_value
                     focused=(
                       switch (state.focusedBlock) {
                       | None => None
                       | Some((id, _blockTyp, changeTyp)) =>
                         id == b_id ? Some(changeTyp) : None
                       }
                     )
                     onChange=(
                       newValue => send(Block_UpdateValue(b_id, newValue))
                     )
                     onExecute=(() => send(Block_Execute(b_id)))
                     onFocus=(() => send(Block_Focus(b_id, BTyp_Code)))
                     onBlockUp=(() => send(Block_FocusUp(b_id)))
                     onBlockDown=(() => send(Block_FocusDown(b_id)))
                     widgets=bc_widgets
                     firstLineNumber=bc_firstLineNumber
                   />
                 </div>
                 <div className="cell__controls">
                   (blockControlsButtons(b_id, send))
                   (
                     /*
                      Display hint on focusedBlock or last CodeBlock
                      If there are no CodeBlock then don't display anything
                      */
                     switch (state.focusedBlock) {
                     | Some((focusedBlock, BTyp_Code, _)) =>
                       focusedBlock == b_id ?
                         blockHint(b_id, send) : ReasonReact.null
                     | _ =>
                       lastCodeBlockId
                       =>> (
                         last_b_id =>
                           last_b_id == b_id ?
                             blockHint(b_id, send) : ReasonReact.null
                       )
                     }
                   )
                 </div>
               </div>
             | B_Text(text) =>
               <div key=b_id id=b_id className="cell__container">
                 <div className="text-editor">
                   <Editor_TextBlock
                     value=text
                     focused=(
                       switch (state.focusedBlock) {
                       | None => None
                       | Some((id, blockTyp, changeTyp)) =>
                         id == b_id ? Some(changeTyp) : None
                       }
                     )
                     onFocus=(() => send(Block_Focus(b_id, BTyp_Text)))
                     onBlockUp=(() => send(Block_FocusUp(b_id)))
                     onBlockDown=(() => send(Block_FocusDown(b_id)))
                     onChange=(
                       newValue => send(Block_UpdateValue(b_id, newValue))
                     )
                   />
                 </div>
                 <div className="cell__controls">
                   (blockControlsButtons(b_id, send))
                 </div>
               </div>
             }
           )
        |. ReasonReact.array
      )
    </div>;
  },
};
