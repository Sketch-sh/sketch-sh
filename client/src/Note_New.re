let blocks = [|
  {
    Editor_Types.Block.b_id: Utils.generateId(),
    b_data: Editor_Blocks_Utils.emptyCodeBlock(),
  },
|];

let component = ReasonReact.statelessComponent("Note_New");

let make = _children => {
  ...component,
  render: _self =>
    <AuthStatus.IsAuthenticated>
      ...(
           isLogin => {
             let userId =
               switch (isLogin) {
               | None => Config.anonymousUserId
               | Some(userId) => userId
               };
             <NoteSave noteKind=New>
               ...(
                    (~loading, ~onSave) =>
                      <Editor_Note blocks loading onSave=(onSave(~userId)) />
                  )
             </NoteSave>;
           }
         )
    </AuthStatus.IsAuthenticated>,
};
