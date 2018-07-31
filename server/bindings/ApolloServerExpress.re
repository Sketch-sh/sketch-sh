module Options = {
  [@bs.deriving abstract]
  type t = {schema: Graphql.schema};

  let make = t;
};

type t;

[@bs.new] [@bs.module "apollo-server-express"]
external makeApolloServer: Options.t => t = "ApolloServer";

[@bs.deriving abstract]
type applyMiddlewareOptions = {
  app: Express.App.t,
  [@bs.optional]
  path: string,
};

[@bs.send] external applyMiddleware: (t, applyMiddlewareOptions) => unit = "";
