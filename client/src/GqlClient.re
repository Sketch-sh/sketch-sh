[@bs.send]
external concat :
  (ReasonApolloTypes.apolloLink, ReasonApolloTypes.apolloLink) =>
  ReasonApolloTypes.apolloLink =
  "concat";

let inMemoryCache = ApolloInMemoryCache.createInMemoryCache();

/* Create an HTTP Link */
let httpLink = ApolloLinks.createHttpLink(~uri=Config.graphqlEndpoint, ());

let authLink =
  ApolloLinks.createContextLink(() => {
    let token = Auth.Auth.getToken();
    switch (token) {
    | None => Js.Obj.empty()
    | Some(token) => {
        "headers": {
          "authorization": "Bearer " ++ token,
        },
      }
    };
  });

let link = authLink |. concat(httpLink);

let instance =
  ReasonApollo.createApolloClient(~link, ~cache=inMemoryCache, ());
