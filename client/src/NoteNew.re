let defaultBlocks = [|
  {
    Editor_Types.Block.b_id: Utils.generateId(),
    b_data: Editor_Blocks_Utils.emptyCodeBlock(RE),
  },
|];

let component = ReasonReact.statelessComponent("Note_New");

let make = (~blocks=defaultBlocks, ~title=?, _children) => {
  ...component,
  render: _self =>
    <NoteSave noteKind=New>
      ...(
           (~noteSaveStatus, ~user as _, ~onSave) =>
             <Editor_Note
               blocks
               ?title
               noteSaveStatus
               onSave
               isEditable=true
             />
         )
    </NoteSave>,
};
