open Utils_GraphqlPpx;
open GqlUpdateNote;

module AddNoteLoginGql = [%graphql
  {|
    mutation (
      $noteId: String!,
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
    }
  |}
];

module AddNoteLoginComponent = ReasonApollo.CreateMutation(AddNoteLoginGql);

open NoteSave_Types;

let component = ReasonReact.statelessComponent("NoteSave_Anonymous");

let make = (~kind, ~userId, ~onSaveNewNote, ~onSaveOldNote, children) => {
  ...component,
  render: _self =>
    switch (kind) {
    | New =>
      <AddNoteLoginComponent>
        ...(
             (mutation, createNoteResult) => {
               let {AddNoteLoginComponent.result} = createNoteResult;
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
                 ~user=AuthStatus.Login(userId),
                 ~onSave=(~title, ~data) => {
                   let data = data->Editor_Json.V1.JsonEncode.encode;
                   let noteId = Utils.generateId();
                   let newNote =
                     AddNoteLoginGql.make(
                       ~title,
                       ~data,
                       ~noteId,
                       ~userId,
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
      </AddNoteLoginComponent>
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
                 ~user=AuthStatus.Login(userId),
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
    },
};
