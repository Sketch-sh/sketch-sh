open Editor_Types;

let defaultBlocks = [|
  {
    Block.b_id: Utils.generateId(),
    b_data: Editor_Blocks_Utils.emptyCodeBlock(),
    b_deleted: false,
  },
|];

[@react.component]
let make = (~blocks=defaultBlocks, ~title=?, ~links=[||], ~lang, _children) =>
  <AuthStatus.IsAuthenticated>
    ...{authStatus => {
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
    }}
  </AuthStatus.IsAuthenticated>;
