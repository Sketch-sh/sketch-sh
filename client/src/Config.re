open Utils;

let authDomain =
  env == "production" ? "https://rtop-api.khoa.xyz" : "http://localhost:3001";

let graphqlEndpoint =
  env == "production" ?
    "https://rtop-server.herokuapp.com/v1alpha1/graphql" : "/graphql";

let anonymousUserId = "anonymous";

let cmTheme = "rtop";

let stagingHost = "rtop.khoa.xyz";
