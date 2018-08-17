const fs = require("fs");
const fetch = require("node-fetch");
const { Binding } = require("graphql-binding");
const { createHttpLink } = require("apollo-link-http");
const { onError } = require("apollo-link-error");
const { setContext } = require("apollo-link-context");
const { makeRemoteExecutableSchema } = require("graphql-tools");
const jwt = require("jsonwebtoken");

const { graphqlEndpoint, jwtToken } = require("./config");

const selfSignedToken = jwt.sign(
  {
    role: "auth_service",
  },
  jwtToken
);

const httpLink = createHttpLink({
  uri: graphqlEndpoint,
  fetch,
});

const authLink = setContext((_, { headers }) => {
  return {
    headers: {
      ...headers,
      authorization: `Bearer ${selfSignedToken}`,
    },
  };
});

const errorLink = onError(({ graphQLErrors, networkError }) => {
  if (graphQLErrors)
    graphQLErrors.map(({ message, locations, path }) =>
      console.log(
        `[GraphQL error]: Message: ${message}, Location: ${locations}, Path: ${path}`
      )
    );
  if (networkError) {
    console.log(JSON.stringify(networkError, null, 2));
  }
});

const link = errorLink.concat(authLink.concat(httpLink));

class HasuraBinding extends Binding {
  constructor() {
    const schema = makeRemoteExecutableSchema({
      schema: fs.readFileSync(__dirname + "/schema.graphql", "utf-8"),
      link,
    });
    super({ schema });
  }
}

module.exports = {
  HasuraBinding,
  link,
};
