open Utils;

let authDomain = env == "production" ? "/api" : "http://localhost:3001";

let graphqlEndpoint =
  env == "production" ?
    "https://sketch-graphql.herokuapp.com/v1alpha1/graphql" : "/graphql";

let anonymousUserId = "anonymous";

let cmTheme = "rtop";

let stagingHost = "rtop.khoa.xyz";
