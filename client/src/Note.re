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

  let make =
      (~note, ~noteKind, ~noteId, _children)
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
             (~noteSaveStatus, ~userId, ~onSave) => {
               let noteOwnerId =
                 switch (note##owner) {
                 | None => None
                 | Some(owner) =>
                   switch (owner##id) {
                   | None => None
                   | Some(id) => Some(id)
                   }
                 };
               <Editor_Note
                 title=note##title->optionToEmptyString
                 isEditable=false
                 ?noteOwnerId
                 blocks=(
                   switch (note##data) {
                   | None => [||]
                   | Some(blocks) => blocks->Editor_Types.JsonDecode.decode
                   }
                 )
                 noteSaveStatus
                 onSave
               />;
             }
           )
      </NoteSave>,
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
             | Loading => <div> (ReasonReact.string("Loading")) </div>
             | Error(error) =>
               <div> (ReasonReact.string(error##message)) </div>
             | Data(response) =>
               let notes = response##note;
               notes
               ->(
                   arrayFirst(~empty=<NotFound entity="note" />, ~render=note =>
                     <EnsureUrlEncodedData
                       noteId=noteInfo.noteId
                       note
                       noteKind=(Old(noteInfo.noteId))
                     />
                   )
                 );
             }
         )
    </GetNoteComponent>;
  },
};
