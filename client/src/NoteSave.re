open Utils_GraphqlPpx;
module GetNote = [%graphql
  {|
    query getNote (
      $noteId: String!,
      $username: String!
    ) {
      note (
        where: {
          id : {_eq: $noteId},
          owner: {username: {_eq: $username}}
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

module NoteSave = {
  type id = string;
  type noteKind =
    | New
    | Old(id);

  type state = {kind: noteKind};

  type action =
    | SavedNewNote(id);

  /* TODO:
     When receive the mutation result,
     check with the original data to see if they are matched */
  let component = ReasonReact.reducerComponent("NoteSave");

  let make = (~noteKind: noteKind, children) => {
    ...component,
    initialState: () => {kind: noteKind},
    reducer: (action, _state) =>
      switch (action) {
      | SavedNewNote(id) =>
        /* TODO: side effects of transition to new url */
        ReasonReact.Update({kind: Old(id)})
      },
    render: ({state, send}) =>
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
                         ~data=data |. Editor_Types.JsonEncode.encode,
                         ~id=noteId,
                         ~userId,
                         (),
                       );
                     Js.Promise.(
                       mutation(~variables=newNote##variables, ())
                       |> then_(_result =>
                            send(SavedNewNote(noteId)) |. resolve
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
                     let updatedNote =
                       UpdateNoteGql.make(
                         ~title,
                         ~data=data |. Editor_Types.JsonEncode.encode,
                         ~noteId,
                         (),
                       );
                     mutation(
                       ~variables=updatedNote##variables,
                       ~refetchQueries=[|"getNote"|],
                       (),
                     )
                     |> ignore;
                   },
                 );
               }
             )
        </UpdateNoteComponent>
      },
  };
};

include NoteSave;
