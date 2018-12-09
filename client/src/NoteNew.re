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
  render: _self => {
    let%Epitath authStatus = children =>
      <AuthStatus.IsAuthenticated> ...children </AuthStatus.IsAuthenticated>;

    let userId =
      switch (authStatus) {
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
  },
};
