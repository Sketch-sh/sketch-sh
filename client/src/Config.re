open Utils;

let authDomain = env == "production" ? "/api" : "http://localhost:3001";

let stagingHost = "rtop.khoa.xyz";

let graphqlEndpoint =
  env == "production" ?
    Webapi.Dom.(
      location->Location.host == stagingHost ?
        "https://rtop-server.herokuapp.com/v1alpha1/graphql" :
        "https://sketch-graphql.herokuapp.com/v1alpha1/graphql"
    ) :
    "/graphql";

let anonymousUserId = "anonymous";

let cmTheme = "rtop";
