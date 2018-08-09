[@bs.send]
external concat:
  (ReasonApolloTypes.apolloLink, ReasonApolloTypes.apolloLink) =>
  ReasonApolloTypes.apolloLink =
  "concat";

let inMemoryCache = ApolloInMemoryCache.createInMemoryCache();

/* Create an HTTP Link */
let httpLink = ApolloLinks.createHttpLink(~uri=Config.graphqlEndpoint, ());

let authLink =
  ApolloLinks.createContextLink(() => {
    let token = Auth.Auth.Token.get();
    let headers = Js.Dict.empty();

    switch (token) {
    | None =>
      switch (Auth.Auth.EditToken.get()) {
      | None => ()
      | Some(editToken) =>
        headers->(Js.Dict.set("X-Hasura-Edit-Token", editToken))
      }
    | Some(token) =>
      headers->(Js.Dict.set("authorization", "Bearer " ++ token))
    };

    {"headers": headers};
  });

let link = authLink->(concat(httpLink));

let instance =
  ReasonApollo.createApolloClient(~link, ~cache=inMemoryCache, ());
