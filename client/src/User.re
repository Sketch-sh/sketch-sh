Modules.require("./User.css");
open Utils;

module GetNotes = [%graphql
  {|
    query getNotes($userName: String!) {
      note(
        where: {owner: {username: {_eq: $userName}}}
        order_by: created_at_desc
      ) {
        id
        title
        updated_at
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

    <GetNotesComponent variables=notesQuery##variables>
      ...(
           ({result}) =>
             switch (result) {
             | Loading => <UI_FullpageLoading />
             | Error(error) =>
               <div> (ReasonReact.string(error##message)) </div>
             | Data(response) =>
               let notes = response##note;
               <div className="Layout__withHeader User">
                 <h1> {j|$(userName)'s sketchbooks|j}->str </h1>
                 <ul className="User__notes">
                   ...notes
                      ->(
                          Belt.Array.mapU((. note) =>
                            <li className="User__notes__note">
                              <Router.Link
                                className="User__notes--link"
                                route=(
                                  Route.Note({noteId: note##id, data: None})
                                )>
                                <span className="User__notes--title">
                                  (
                                    switch (note##title) {
                                    | None
                                    | Some("") => "untitled sketch"->str
                                    | Some(title) => title->str
                                    }
                                  )
                                </span>
                                <span className="User__notes--dash">
                                  " - "->str
                                </span>
                                ("/s/" ++ note##id)->str
                              </Router.Link>
                              <div className="User__notes__lastEdited">
                                "last edited"->str
                                <UI_DateTime
                                  date=note##updated_at
                                  className="User__notes__time"
                                />
                              </div>
                            </li>
                          )
                        )
                 </ul>
               </div>;
             }
         )
    </GetNotesComponent>;
  },
};
