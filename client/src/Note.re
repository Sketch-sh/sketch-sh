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
        data
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

/*
 * This module will ensure url encoded data always in the URL
 * on initial note load
 */
module EnsureUrlEncodedData = {
  type action =
    | NoteLoaded(Js.Json.t);
  let component = ReasonReact.reducerComponent("Note_EnsureUrlEncodedData");

  let make =
      (~note, ~noteKind, ~noteId, ~userId, _children)
      : React.component(unit, 'a, action) => {
    ...component,
    didMount: ({send}) =>
      switch (note##data) {
      | None => ()
      | Some(json) => send(NoteLoaded(json))
      },
    reducer: (action, _) =>
      switch (action) {
      | NoteLoaded(json) =>
        ReasonReact.SideEffects(
          (
            _ =>
              NoteSave.replaceNoteRoute(
                ~noteId,
                ~json,
                ~title=note##title->optionToEmptyString,
              )
              ->ignore
          ),
        )
      },
    render: _send =>
      <NoteSave noteKind>
        ...(
             (~loading, ~onSave) =>
               <Editor_Note
                 title=note##title->optionToEmptyString
                 blocks=(
                   switch (note##data) {
                   | None => [||]
                   | Some(blocks) => blocks->Editor_Types.JsonDecode.decode
                   }
                 )
                 loading
                 onSave=(onSave(~userId))
               />
           )
      </NoteSave>,
  };
};

let component = ReasonReact.statelessComponent("Note");

let make = (~noteInfo: Route.noteRouteConfig, _children) => {
  ...component,
  render: _self => {
    let noteQuery = GetNote.make(~noteId=noteInfo.noteId, ());
    <AuthStatus.IsAuthenticated>
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
                        ->(
                            arrayFirst(
                              ~empty=<NotFound entity="note" />, ~render=note =>
                              <EnsureUrlEncodedData
                                noteId=noteInfo.noteId
                                note
                                noteKind=(Old(noteInfo.noteId))
                                userId
                              />
                            )
                          );
                      }
                  )
             </GetNoteComponent>;
           }
         )
    </AuthStatus.IsAuthenticated>;
  },
};
