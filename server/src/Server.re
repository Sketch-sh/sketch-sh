open! Express;

let isProduction = Utils.env == "production";

let rtopServer = Express.App.make();

let port = 3001;

App.use(rtopServer, Morgan.(make(isProduction ? Combined : Dev)));

App.get(
  rtopServer,
  ~path="/healthz",
  Middleware.from((_, _) => Response.sendString("ok")),
);

App.get(
  rtopServer,
  ~path="/",
  Middleware.from((_, _) => Response.sendString("Hello world")),
);

let apolloServer = {
  open ApolloServerExpress;
  let schema = Graphql.(makeSchema(toField({"viewer": Schema.jsSchema})));
  makeApolloServer(Options.make(~schema));
};

ApolloServerExpress.(
  apolloServer
  ->(applyMiddleware(applyMiddlewareOptions(~app=rtopServer, ())))
);

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ => Js.log @@ "Listening at http://localhost:3001"
  };

let server = App.listen(rtopServer, ~port, ~onListen, ());
