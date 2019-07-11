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
        returning {
          __typename
          id
          title
          data
          updated_at
          user_id
          fork_from
        }
      }
    }
  |}
];

module UpdateNoteComponent = ReasonApollo.CreateMutation(UpdateNoteGql);
