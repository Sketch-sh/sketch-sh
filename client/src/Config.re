open Utils;

let authDomain =
  env == "production" ? "http://localhost:3001" : "http://localhost:3001";

let graphqlEndpoint = "http://localhost:8081/v1alpha1/graphql";
