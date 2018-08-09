open Utils_GraphqlPpx;

module UpdateNoteGql = [%graphql
  {|
    mutation ($noteId: String!, $data: jsonb!, $title: String!) {
      mutate: update_note(
        where: {
          id: {_eq: $noteId}
        }
        _set: {
        title: $title,
        data: $data
      }) {
        affected_rows
      }
    }
  |}
];

module UpdateNoteComponent = ReasonApollo.CreateMutation(UpdateNoteGql);
