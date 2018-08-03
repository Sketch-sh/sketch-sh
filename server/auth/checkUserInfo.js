const { execute, makePromise } = require("apollo-link");
const { link, HasuraBinding } = require("./hasura-binding");
const gql = require("graphql-tag");

const hasuraInstance = new HasuraBinding();

const query = hasuraInstance.query;
const mutation = hasuraInstance.mutation;

exports.githubCheckIdentity = id =>
  query.user_identity(
    {
      where: {
        identity_type: { _eq: "github" },
        identity_id: { _eq: id },
      },
    },
    `{ user_id }`
  );

const makeCreateGithubIdentityOperation = variables => ({
  query: gql`
    mutation createGithubIdentity(
      $user_info: [user_insert_input!]!
      $user_identity: [user_identity_insert_input!]!
    ) {
      insert_user(objects: $user_info) {
        returning {
          id
        }
      }

      insert_user_identity(objects: $user_identity) {
        affected_rows
      }
    }
  `,
  variables,
});

exports.createGithubIdentity = ({
  user_id,
  github_id,
  username,
  name,
  email,
  data,
  avatar,
}) => {
  const user_info = {
    id: user_id,
    email,
    name,
    username,
    avatar,
  };
  const user_identity = {
    identity_type: "github",
    identity_id: github_id,
    user_id,
    data,
  };

  return makePromise(
    execute(
      link,
      makeCreateGithubIdentityOperation({ user_info, user_identity })
    )
  );
};
