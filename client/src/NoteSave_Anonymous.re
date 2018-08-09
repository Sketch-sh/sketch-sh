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

      insert_note_edit_token(objects:{
        note_id: $noteId
        token: $editToken
      }) {
        affected_rows
      }

      insert_note(objects: {
        title: $title,
        id: $noteId,
        user_id: $userId,
        data: $data
      }) {
        returning {
          updated_at
        }
      }
    }
  |}
];

module AddNoteAnonymousComponent =
  ReasonApollo.CreateMutation(AddNoteAnonymousGql);

open NoteSave_Types;

let component = ReasonReact.statelessComponent("NoteSave_Anonymous");

let make = (~kind, ~user, ~onSaveNewNote, ~onSaveOldNote, children) => {
  ...component,
  render: _self =>
    switch (kind) {
    | New =>
      <AddNoteAnonymousComponent>
        ...(
             (mutation, createNoteResult) => {
               let {AddNoteAnonymousComponent.loading} = createNoteResult;
               children(
                 ~loading,
                 ~user,
                 ~onSave=(~title, ~data) => {
                   let noteId = Utils.generateId();
                   let newNote =
                     AddNoteAnonymousGql.make(
                       ~title,
                       ~data=data->Editor_Types.JsonEncode.encode,
                       ~noteId,
                       ~userId=Config.anonymousUserId,
                       ~editToken=Auth.Auth.getOrCreateEditToken(),
                       (),
                     );
                   Js.Promise.(
                     mutation(~variables=newNote##variables, ())
                     |> then_(_result =>
                          onSaveNewNote(
                            noteId,
                            title,
                            data->Editor_Types.JsonEncode.encode,
                          )
                          ->resolve
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
               let {UpdateNoteComponent.loading} = updateNoteResult;
               children(
                 ~loading,
                 ~user,
                 ~onSave=(~title, ~data) => {
                   let data = data->Editor_Types.JsonEncode.encode;
                   let updatedNote =
                     UpdateNoteGql.make(~title, ~data, ~noteId, ());
                   Js.Promise.(
                     mutation(
                       ~variables=updatedNote##variables,
                       ~refetchQueries=[|
                         {
                           "query": "getNote",
                           "variables": {
                             "where": {
                               "id": {
                                 "_eq": noteId,
                               },
                             },
                           },
                         }
                         ->Utils_GraphqlPpx.hackRefetchQueries,
                       |],
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
