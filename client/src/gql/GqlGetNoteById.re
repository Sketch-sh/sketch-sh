module GetNoteById = [%graphql
  {|
    query getNoteById (
      $noteId: String!
    ) {
      note: note (where: {id : {_eq: $noteId}}) {
        ...GqlFragment.Editor.EditorNote
      }
      note_edit_token(where: {note_id: {_eq: $noteId}}) {
        note_id
      }
    }
  |}
];

module GetNoteByIdComponent = ReasonApollo.CreateQuery(GetNoteById);
