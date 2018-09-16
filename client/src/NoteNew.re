open Editor_Types;

let defaultBlocks = [|
  {
    Block.b_id: Utils.generateId(),
    b_data: Editor_Blocks_Utils.emptyCodeBlock(),
    b_deleted: false,
  },
|];

let component = ReasonReact.statelessComponent("Note_New");

let make = (~blocks=defaultBlocks, ~title=?, ~lang, ~links=[||], _children) => {
  ...component,
  render: _self =>
    <AuthStatus.IsAuthenticated>
      ...(
           user => {
             let userId =
               switch (user) {
               | Anonymous => Config.anonymousUserId
               | Login(userId) => userId
               };
             let noteId = Utils.generateId();
             <Editor_Note
               key=noteId
               hasSavePermission=true
               noteOwnerId=userId
               noteLastEdited=None
               noteId
               noteState=NoteState_New
               blocks
               lang
               ?title
               links
             />;
           }
         )
    </AuthStatus.IsAuthenticated>,
};
