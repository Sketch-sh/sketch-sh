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
    | NoteLoaded(Js.Json.t);
  let component = ReasonReact.reducerComponent("Note_EnsureUrlEncodedData");

  let make = (~note, ~noteId, children): React.component(unit, 'a, action) => {
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
                ~kind=Replace,
              )
              ->ignore
          ),
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
             | Loading => <UI_FullpageLoading />
             | Error(error) =>
               <div> (ReasonReact.string(error##message)) </div>
             | Data(response) =>
               let notes = response##note;
               notes
               ->(
                   arrayFirst(~empty=<NotFound entity="note" />, ~render=note =>
                     <EnsureUrlEncodedData noteId=noteInfo.noteId note>
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
                                     ?noteOwnerId
                                     blocks=(
                                       switch (note##data) {
                                       | None => [||]
                                       | Some(blocks) =>
                                         blocks->Editor_Types.JsonDecode.decode
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
