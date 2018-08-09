open Utils_GraphqlPpx;

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
