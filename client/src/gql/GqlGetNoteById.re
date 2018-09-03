module GetNoteById = [%graphql
  {|
    query getNoteById (
      $noteId: String!
    ) {
      note: note (where: {id : {_eq: $noteId}}) {
        __typename
        id
        title
        data
        updated_at
        user_id
        fork_from
      }
      note_edit_token(where: {note_id: {_eq: $noteId}}) {
        note_id
      }
    }
  |}
];

module GetNoteByIdComponent = ReasonApollo.CreateQuery(GetNoteById);
