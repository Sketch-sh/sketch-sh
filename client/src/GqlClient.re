[@bs.send]
external concat:
  (ReasonApolloTypes.apolloLink, ReasonApolloTypes.apolloLink) =>
  ReasonApolloTypes.apolloLink =
  "concat";

external toString: Js.Nullable.t('a) => string = "%identity";

let cache =
  ApolloInMemoryCache.createInMemoryCache(
    ~dataIdFromObject=
      obj =>
        (
          switch (obj##id->Js.Nullable.toOption) {
          | None => Js.Nullable.null
          | Some(id) => Js.Nullable.return(id)
          }
        )
        |> toString,
    (),
  );

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

let instance = ReasonApollo.createApolloClient(~link, ~cache, ());
