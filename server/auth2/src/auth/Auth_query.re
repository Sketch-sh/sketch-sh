module Get_user_from_github_id = [%graphql
  {|
query get_user_from_github_id($github_id: String!) {
  user_identity(where: {
    _and: {
      identity_id: {
        _eq: $github_id
      },
      identity_type: {
        _eq: "github"
      }
    }
  }) {
    user_id
  }
}
|}
];
