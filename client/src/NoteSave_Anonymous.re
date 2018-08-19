open Utils_GraphqlPpx;
open GqlUpdateNote;

module AddNoteAnonymousGql = [%graphql
  {|
    mutation (
      $noteId: String!,
      $editToken: String!,
      $userId: String!,
      $title: String!,
      $data: jsonb!
    ) {
      mutate: insert_note(objects: {
        title: $title,
        id: $noteId,
        user_id: $userId,
        data: $data
      }) {
        affected_rows
      }
      insert_note_edit_token(objects:{
        note_id: $noteId
        token: $editToken
      }) {
        affected_rows
      }
    }
  |}
];

module AddNoteAnonymousComponent =
  ReasonApollo.CreateMutation(AddNoteAnonymousGql);

open NoteSave_Types;

let component = ReasonReact.statelessComponent("NoteSave_Anonymous");

let make = (~kind, ~onSaveNewNote, ~onSaveOldNote, children) => {
  ...component,
  render: _self => {
    let userId = Config.anonymousUserId;
    switch (kind) {
    | New =>
      <AddNoteAnonymousComponent>
        ...(
             (mutation, createNoteResult) => {
               let {AddNoteAnonymousComponent.result} = createNoteResult;

               children(
                 ~noteSaveStatus=
                   switch (result) {
                   | Loading => NoteSave_Loading
                   | Error(_apolloError) => NoteSave_Error
                   | Data(data) =>
                     switch (data##mutate) {
                     | None => NoteSave_Error
                     | Some(mutate) =>
                       if (mutate##affected_rows > 0) {
                         NoteSave_Done;
                       } else {
                         NoteSave_Error;
                       }
                     }

                   | NotCalled => NoteSave_Done
                   },
                 ~user=AuthStatus.Anonymous,
                 ~onSave=(~title, ~data) => {
                   let data = data->Editor_Json.V1.JsonEncode.encode;
                   let noteId = Utils.generateId();
                   let newNote =
                     AddNoteAnonymousGql.make(
                       ~title,
                       ~data,
                       ~noteId,
                       ~userId,
                       ~editToken=Auth.Auth.getOrCreateEditToken(),
                       (),
                     );
                   Js.Promise.(
                     mutation(~variables=newNote##variables, ())
                     |> then_(_result =>
                          onSaveNewNote(noteId, title, data)->resolve
                        )
                   )
                   |> ignore;
                 },
               );
             }
           )
      </AddNoteAnonymousComponent>
    | Old(noteId) =>
      <UpdateNoteComponent>
        ...(
             (mutation, updateNoteResult) => {
               let {UpdateNoteComponent.result} = updateNoteResult;
               children(
                 ~noteSaveStatus=
                   switch (result) {
                   | Loading => NoteSave_Loading
                   | Error(_apolloError) => NoteSave_Error
                   | Data(data) =>
                     switch (data##mutate) {
                     | None => NoteSave_Error
                     | Some(mutate) =>
                       if (mutate##affected_rows > 0) {
                         NoteSave_Done;
                       } else {
                         NoteSave_Error;
                       }
                     }

                   | NotCalled => NoteSave_Done
                   },
                 ~user=AuthStatus.Anonymous,
                 ~onSave=(~title, ~data) => {
                   let data = data->Editor_Json.V1.JsonEncode.encode;
                   let updatedNote =
                     UpdateNoteGql.make(~title, ~data, ~noteId, ());
                   Js.Promise.(
                     mutation(
                       ~variables=updatedNote##variables,
                       ~refetchQueries=[|"getNote"|],
                       (),
                     )
                     |> then_(_data =>
                          onSaveOldNote(noteId, title, data)->resolve
                        )
                   )
                   |> ignore;
                 },
               );
             }
           )
      </UpdateNoteComponent>
    };
  },
};
