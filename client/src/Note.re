module GetNote = [%graphql
  {|
    query getNote (
      $noteId: String!
    ) {
      note (where: {id : {_eq: $noteId}}) {
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
      note_edit_token(where: {note_id: {_eq: $noteId}}) {
        note_id
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
    | NoteLoaded;
  let component = ReasonReact.reducerComponent("Note_EnsureUrlEncodedData");

  let make = (~noteId, children): React.component(unit, 'a, action) => {
    ...component,
    didMount: ({send}) => send(NoteLoaded),
    reducer: (action, _) =>
      switch (action) {
      | NoteLoaded =>
        ReasonReact.SideEffects(
          (_ => NoteSave.replaceNoteRoute(~noteId, ~kind=Replace)->ignore),
        )
      },
    render: _send => children,
  };
};

let component = ReasonReact.statelessComponent("Note");

let make = (~noteInfo: Route.noteRouteConfig, _children) => {
  ...component,
  render: _self => {
    let noteQuery = GetNote.make(~noteId=noteInfo.noteId, ());

    <GetNoteComponent variables=noteQuery##variables>
      ...(
           ({result}) =>
             switch (result) {
             | Loading => <Editor_NotePlaceholder />
             | Error(error) => error##message->str
             | Data(response) =>
               let notes = response##note;
               notes
               ->(
                   arrayFirst(~empty=<NotFound entity="note" />, ~render=note =>
                     <EnsureUrlEncodedData noteId=noteInfo.noteId>
                       ...<NoteSave noteKind=(Old(noteInfo.noteId))>
                            ...(
                                 (~noteSaveStatus, ~user, ~onSave) => {
                                   let noteOwnerId =
                                     switch (note##owner##id) {
                                     | None => None
                                     | Some(id) => Some(id)
                                     };

                                   let isEditable =
                                     switch (user) {
                                     | Login(currentUserId) =>
                                       switch (noteOwnerId) {
                                       | None => false
                                       | Some(noteOwnerId) =>
                                         noteOwnerId == currentUserId
                                       }
                                     | Anonymous =>
                                       response##note_edit_token->Array.length
                                       > 0
                                     };
                                   <Editor_Note
                                     title=note##title->optionToEmptyString
                                     isEditable
                                     noteLastEdited=note##updated_at
                                     noteOwner=note##owner
                                     blocks=(
                                       switch (note##data) {
                                       | None => [||]
                                       | Some(blocks) =>
                                         blocks
                                         ->Editor_Json.V1.JsonDecode.decode
                                       }
                                     )
                                     noteSaveStatus
                                     onSave
                                   />;
                                 }
                               )
                          </NoteSave>
                     </EnsureUrlEncodedData>
                   )
                 );
             }
         )
    </GetNoteComponent>;
  },
};
