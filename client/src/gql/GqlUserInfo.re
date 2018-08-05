module UserInfoGql = [%graphql
  {|
    query userInfo($userId: String!) {
      user_public(where: {id: {_eq: $userId}}) {
        id
        name
        username
        avatar
      }
    }
  |}
];

module UserInfoComponent = ReasonApollo.CreateQuery(UserInfoGql);
