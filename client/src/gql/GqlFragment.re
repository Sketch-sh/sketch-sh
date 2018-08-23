module Editor = [%graphql
  {|
    fragment editorNote on note {
      __typename
      id
      title
      data
      updated_at
      user_id
    }
  |}
];
module User = [%graphql
  {|
    fragment userInfo on user_public {
      __typename
      id
      name
      username
      avatar
    }
  |}
];
