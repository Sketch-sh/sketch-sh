open Utils;

let authDomain =
  env == "production"
    ? "https://api.sketch.sh/auth" : "http://localhost:3000/api/auth";

let stagingHost = "staging.sketch.sh";
let productionHost = "sketch-sh.github.io";
let graphqlEndpoint =
  env == "production"
    ? {
      open Webapi.Dom;
      let host = location->Location.host;

      if (host == stagingHost) {
        "https://rtop-server.herokuapp.com/v1alpha1/graphql";
      } else {
        "https://api.sketch.sh/graphql";
      };
    }
    : "/graphql";

let anonymousUserId = "anonymous";

let cmTheme = "rtop";

let sketchListLimit = 30;
