Modules.require("./User.css");
open Utils;

module GetNotes = [%graphql
  {|
    query getNotes($userName: String!) {
      note(
        where: {owner: {username: {_eq: $userName}}}
        order_by: updated_at_desc
      ) {
        id
        title
        date: updated_at
      }
    }
  |}
];

module GetNotesComponent = ReasonApollo.CreateQuery(GetNotes);

let component = ReasonReact.statelessComponent("User");

let make = (~userName, _children) => {
  ...component,
  render: _self => {
    let notesQuery = GetNotes.make(~userName, ());
    <section className="Layout__center User">
      <h1> {j|$(userName)'s sketches|j}->str </h1>
      <GetNotesComponent variables=notesQuery##variables>
        ...(
             ({result}) =>
               switch (result) {
               | Loading =>
                 <div style=(ReactDOMRe.Style.make(~width="500px", ()))>
                   <UI_SketchList.Placeholder />
                 </div>
               | Error(error) => error##message->str
               | Data(response) =>
                 <UI_SketchList
                   sketches=response##note
                   noSketches={<UI_NoSketches />}
                 />
               }
           )
      </GetNotesComponent>
    </section>;
  },
};
