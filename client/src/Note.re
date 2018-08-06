open Utils_GraphqlPpx;

module GetNote = [%graphql
  {|
    query getNote (
      $noteId: String!
    ) {
      note (
        where: {
          id : {_eq: $noteId}
        }
      ) {
        id
        title
        data @bsDecoder(fn: "decodeBlockData")
        updated_at
        owner {
          id
          username
          avatar
        }
      }
    }
  |}
];

module GetNoteComponent = ReasonApollo.CreateQuery(GetNote);

open Utils;

let component = ReasonReact.statelessComponent("Note");

let make = (~noteInfo: Route.noteRouteConfig, _children) => {
  ...component,
  render: _self => {
    let noteQuery = GetNote.make(~noteId=noteInfo.noteId, ());
    <Auth.IsAuthenticated>
      ...(
           isLogin => {
             let userId =
               switch (isLogin) {
               | None => Config.anonymousUserId
               | Some(userId) => userId
               };
             <GetNoteComponent variables=noteQuery##variables>
               ...(
                    ({result}) =>
                      switch (result) {
                      | Loading =>
                        <div> (ReasonReact.string("Loading")) </div>
                      | Error(error) =>
                        <div> (ReasonReact.string(error##message)) </div>
                      | Data(response) =>
                        let notes = response##note;
                        notes
                        |. arrayFirst(
                             ~empty=<NotFound entity="note" />, ~render=note =>
                             <NoteSave noteKind=(Old(noteInfo.noteId))>
                               ...(
                                    (~loading, ~onSave) =>
                                      <Editor_Note
                                        blocks=(
                                          switch (note##data) {
                                          | None => [||]
                                          | Some(blocks) => blocks
                                          }
                                        )
                                        loading
                                        onSave=(onSave(~userId))
                                      />
                                  )
                             </NoteSave>
                           );
                      }
                  )
             </GetNoteComponent>;
           }
         )
    </Auth.IsAuthenticated>;
  },
};
