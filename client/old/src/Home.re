open Utils;

module GetRecentSketches = [%graphql
  {|
    query getRecentSketches($userId: String!) {
      sketches: note(
        where: {user_id: {_eq: $userId}}
        order_by: {
          updated_at: desc
        },
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
        order_by: {
          created_at: desc
        },
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

[@react.component]
let make = (~userId, ()) =>
  ReactCompat.useRecordApi({
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
            ...{({result}) =>
              <section>
                <h2 className="Home__section--title">
                  "Your recent sketches"->str
                </h2>
                {switch (result) {
                 | Loading =>
                   <UI_SketchList.Placeholder
                     className="Home__section--list"
                   />
                 | Error(error) => error##message->str
                 | Data(response) =>
                   <UI_SketchList
                     sketches=response##sketches
                     className="Home__section--list"
                     noSketches={<UI_NoSketches />}
                   />
                 }}
              </section>
            }
          </GetRecentSketchesComponent>
          <GetCommunitySketchesComponent
            variables=recentSketchesQuery##variables>
            ...{({result}) =>
              <section>
                <h2 className="Home__section--title">
                  "New community sketches"->str
                </h2>
                {switch (result) {
                 | Loading =>
                   <UI_SketchList.Placeholder
                     className="Home__section--list"
                   />
                 | Error(error) => error##message->str
                 | Data(response) =>
                   <UI_SketchList.WithUserInfo
                     sketches=response##sketches
                     className="Home__section--list"
                   />
                 }}
              </section>
            }
          </GetCommunitySketchesComponent>
        </main>
      </div>;
    },
  });
