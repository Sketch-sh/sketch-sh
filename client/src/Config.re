open Utils;

let authDomain = "https://api.sketch.sh/auth";

let stagingHost = "staging.sketch.sh";
let productionHost = "sketch.sh";
let graphqlEndpoint =
  env == "production"
    ? {
      open Webapi.Dom;
      let host = location->Location.host;

      if (host == stagingHost) {
        "https://rtop-server.herokuapp.com/v1alpha1/graphql";
      } else if (host == productionHost) {
        "https://api.sketch.sh/graphql";
      } else {
        "/graphql";
      };
    }
    : "/graphql";

let anonymousUserId = "anonymous";

let cmTheme = "rtop";

let sketchListLimit = 30;
