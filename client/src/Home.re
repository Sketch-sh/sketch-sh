open Utils;

module GetRecentSketches = [%graphql
  {|
    query getRecentSketches($userId: String!) {
      sketches: note(
        where: {user_id: {_eq: $userId}}
        order_by: updated_at_desc
        limit: 5
      ) {
        id
        title
        date: updated_at
      }
    }
  |}
];

module GetRecentSketchesComponent =
  ReasonApollo.CreateQuery(GetRecentSketches);

module GetCommunitySketches = [%graphql
  {|
    query getCommunitySketches($userId: String!) {
      sketches: note(
        where: {
          _and: [
            {user_id: {_neq: $userId}},
            {user_id: {_neq: "anonymous"}}
          ]
        },
        order_by: created_at_desc,
        limit: 5
      ) {
        id
        title
        date: created_at
        owner {
          id
          avatar
          username
        }
      }
    }
  |}
];
module GetCommunitySketchesComponent =
  ReasonApollo.CreateQuery(GetCommunitySketches);

Modules.require("./Home.css");

let component = ReasonReact.statelessComponent("Home");

let make = (~userId, _children) => {
  ...component,
  render: _self => {
    let recentSketchesQuery = GetRecentSketches.make(~userId, ());

    <div className="Layout__center Home">
      <Helmet>
        <title> "Sketch.sh - Interactive ReasonML sketchbook"->str </title>
      </Helmet>
      <h1> "Home"->str </h1>
      <main className="Home__main">
        <GetRecentSketchesComponent variables=recentSketchesQuery##variables>
          ...(
               ({result}) =>
                 switch (result) {
                 | Loading => <UI_FullpageLoading />
                 | Error(error) =>
                   <section> (ReasonReact.string(error##message)) </section>
                 | Data(response) =>
                   <section>
                     <h2 className="Home__section--title">
                       "Your recent sketches"->str
                     </h2>
                     <UI_LoadingPlaceholder className="Home__section--list">
                       <UI_LoadingPlaceholder.Rect
                         top="0"
                         right="0"
                         width="40%"
                         height="20px"
                       />
                       <UI_LoadingPlaceholder.Rect
                         top="20px"
                         left="0"
                         right="0"
                         height="5px"
                       />
                       <UI_LoadingPlaceholder.Rect
                         top="25px"
                         left="30px"
                         width="10px"
                         height="10px"
                       />
                       <UI_LoadingPlaceholder.Rect
                         top="25px"
                         left="200px"
                         width="15px"
                         height="10px"
                       />
                       <UI_LoadingPlaceholder.Rect
                         top="25px"
                         left="450px"
                         right="0"
                         height="10px"
                       />
                       <UI_LoadingPlaceholder.Rect
                         top="35px"
                         left="0"
                         right="0"
                         height="5px"
                       />
                     </UI_LoadingPlaceholder>
                   </section>
                 }
             )
        </GetRecentSketchesComponent>
        <GetCommunitySketchesComponent
          variables=recentSketchesQuery##variables>
          ...(
               ({result}) =>
                 switch (result) {
                 | Loading => <UI_FullpageLoading />
                 | Error(error) =>
                   <section> (ReasonReact.string(error##message)) </section>
                 | Data(response) =>
                   <section>
                     <h2 className="Home__section--title">
                       "New community sketches"->str
                     </h2>
                     <UI_SketchList.WithUserInfo
                       sketches=response##sketches
                       className="Home__section--list"
                     />
                   </section>
                 }
             )
        </GetCommunitySketchesComponent>
      </main>
    </div>;
  },
};
