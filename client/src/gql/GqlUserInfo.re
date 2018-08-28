module UserInfoGql = [%graphql
  {|
    query userInfo($userId: String!) {
      user_public(where: {id: {_eq: $userId}}) {
        ...GqlFragment.User.UserInfo
      }
    }
  |}
];

module UserInfoComponent = ReasonApollo.CreateQuery(UserInfoGql);
