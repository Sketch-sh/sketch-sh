const fs = require("fs");
const fetch = require("node-fetch");
const { Binding } = require("graphql-binding");
const { createHttpLink } = require("apollo-link-http");
const { makeRemoteExecutableSchema } = require("graphql-tools");

const { concat } = require("apollo-link");
const { HttpLink } = require("apollo-link-http");
const { ErrorLink, onError } = require("apollo-link-error");

const graphqlEndpoint = process.env.HASURA_ENDPOINT;

const httpLink = createHttpLink({
  uri: graphqlEndpoint,
  fetch,
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

const link = errorLink.concat(httpLink);

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
