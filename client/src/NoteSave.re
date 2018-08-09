open Utils_GraphqlPpx;

module AddNoteGql = [%graphql
  {|
    mutation ($title: String!, $data: jsonb!, $id: String!, $userId: String!) {
      addNote: insert_note(objects: {
        title: $title,
        id: $id,
        user_id: $userId,
        data: $data
      }) {
        returning {
          id
          updated_at
          title
          data
        }
      }
    }
  |}
];

module AddNoteComponent = ReasonApollo.CreateMutation(AddNoteGql);

module UpdateNoteGql = [%graphql
  {|
    mutation ($noteId: String!, $data: jsonb!, $title: String!) {
      update_note(
        where: {
          id: {_eq: $noteId}
        }
        _set: {
        title: $title,
        data: $data
      }) {
        returning {
          updated_at
        }
      }
    }
  |}
];

module UpdateNoteComponent = ReasonApollo.CreateMutation(UpdateNoteGql);

let replaceNoteRoute = (~noteId, ~json, ~title) =>
  Js.Promise.(
    LzString.async()
    |> then_(lzstring =>
         (
           lzstring->(LzString.URI.compress(title))
           ++ "---"
           ++ lzstring->LzString.URI.compress(Js.Json.stringify(json))
         )
         ->resolve
       )
    |> then_(compressed =>
         Router.pushSilent(Route.Note({noteId, data: Some(compressed)}))
         ->resolve
       )
    |> Utils.handleError
  );
module NoteSave = {
  open NoteSave_Types;
  type action =
    | SavedNewNote(id, string, Js.Json.t)
    | SavedOldNote(id, string, Js.Json.t);

  /* TODO:
     When receive the mutation result,
     check with the original data to see if they are matched */
  let component = ReasonReact.reducerComponent("NoteSave");

  let make = (~noteKind: noteKind, children) => {
    ...component,
    initialState: () => {kind: noteKind},
    reducer: (action, _state) =>
      switch (action) {
      | SavedNewNote(noteId, title, json) =>
        ReasonReact.UpdateWithSideEffects(
          {kind: Old(noteId)},
          (_self => replaceNoteRoute(~noteId, ~json, ~title)->ignore),
        )
      | SavedOldNote(noteId, title, json) =>
        ReasonReact.SideEffects(
          (_self => replaceNoteRoute(~noteId, ~json, ~title)->ignore),
        )
      },
    render: ({state, send}) =>
      <AuthStatus.IsAuthenticated>
        ...(
             user =>
               switch (user) {
               | Anonymous => ReasonReact.null
               | Login(userId) =>
                 switch (state.kind) {
                 | New =>
                   <AddNoteComponent>
                     ...(
                          (mutation, createNoteResult) => {
                            let {AddNoteComponent.loading} = createNoteResult;
                            children(
                              ~loading,
                              ~onSave=(~title, ~data, ~userId) => {
                                let noteId = Utils.generateId();
                                let newNote =
                                  AddNoteGql.make(
                                    ~title,
                                    ~data=data->Editor_Types.JsonEncode.encode,
                                    ~id=noteId,
                                    ~userId,
                                    (),
                                  );
                                Js.Promise.(
                                  mutation(~variables=newNote##variables, ())
                                  |> then_(_result =>
                                       send(
                                         SavedNewNote(
                                           noteId,
                                           title,
                                           data
                                           ->Editor_Types.JsonEncode.encode,
                                         ),
                                       )
                                       ->resolve
                                     )
                                )
                                |> ignore;
                              },
                            );
                          }
                        )
                   </AddNoteComponent>
                 | Old(noteId) =>
                   <UpdateNoteComponent>
                     ...(
                          (mutation, updateNoteResult) => {
                            let {UpdateNoteComponent.loading} = updateNoteResult;
                            children(
                              ~loading,
                              ~onSave=(~title, ~data, ~userId as _) => {
                                let data =
                                  data->Editor_Types.JsonEncode.encode;
                                let updatedNote =
                                  UpdateNoteGql.make(
                                    ~title,
                                    ~data,
                                    ~noteId,
                                    (),
                                  );
                                Js.Promise.(
                                  mutation(
                                    ~variables=updatedNote##variables,
                                    ~refetchQueries=[|"getNote"|],
                                    (),
                                  )
                                  |> then_(_data =>
                                       SavedOldNote(noteId, title, data)
                                       ->send
                                       ->resolve
                                     )
                                )
                                |> ignore;
                              },
                            );
                          }
                        )
                   </UpdateNoteComponent>
                 }
               }
           )
      </AuthStatus.IsAuthenticated>,
  };
};

include NoteSave;
