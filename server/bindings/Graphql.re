type schema;

type fields;

external toField: Js.t('a) => fields = "%identity";

type objTyp;

[@bs.deriving abstract]
type graphQLObjectTypeOptions = {
  name: string,
  fields,
};

[@bs.deriving abstract]
type rootSchema = {query: objTyp};

[@bs.new] [@bs.module "graphql"]
external graphQLSchema: rootSchema => schema = "GraphQLSchema";
[@bs.new] [@bs.module "graphql"]
external graphQLObjectType: graphQLObjectTypeOptions => objTyp =
  "GraphQLObjectType";

let makeSchema = fields =>
  graphQLSchema(
    rootSchema(
      ~query=
        graphQLObjectType(
          graphQLObjectTypeOptions(~name="RootQueryType", ~fields),
        ),
    ),
  );

/* var schema = new GraphQLSchema({
     query: new GraphQLObjectType({
       name: "RootQueryType",
       fields: {
         viewer: jsSchema,
       },
     }),
   }); */
