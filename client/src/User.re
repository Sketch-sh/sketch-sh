Modules.require("./User.css");
open Utils;

module GetNotes = [%graphql
  {|
    query getNotes($userName: String!) {
      note(
        where: { owner: { username: { _eq: $userName } } }
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

    <div className="Layout__withHeader">
      <GetNotesComponent variables=notesQuery##variables>
        ...(
             ({result}) =>
               switch (result) {
               | Loading => <UI_FullpageLoading />
               | Error(error) =>
                 <div> (ReasonReact.string(error##message)) </div>
               | Data(response) =>
                 let notes = response##note;

                 <div>
                   <ul className="User__notes">
                     ...notes
                        ->(
                            Belt.Array.mapU((. note) =>
                              <li className="User__notes__note">
                                <Router.Link
                                  className="User__notes__title"
                                  route=(
                                    Route.Note({noteId: note##id, data: None})
                                  )>
                                  (
                                    switch (note##title) {
                                    | None
                                    | Some("") => "untitled sketch"->str
                                    | Some(title) => title->str
                                    }
                                  )
                                </Router.Link>
                                <span className="User__notes__lastEdited">
                                  "last edited"->str
                                  <UI_DateTime
                                    date=note##updated_at
                                    className="User__notes__time"
                                  />
                                </span>
                              </li>
                            )
                          )
                   </ul>
                 </div>;
               }
           )
      </GetNotesComponent>
    </div>;
  },
};
