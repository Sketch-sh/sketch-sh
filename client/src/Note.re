module GetNote = [%graphql
  {|
  query getNoteInformation (
    $noteId: String!,
    $username: String!
  ) {
    note (
      where: {
        id : {_eq: $noteId},
        owner: {username: {_eq: $username}}
      }
    ) {
      id
      title
      updated_at
      owner {
        id
        username
        avatar
      }
    }
  }
|}
];

module GetNoteQuery = ReasonApollo.CreateQuery(GetNote);

open Utils;

let component = ReasonReact.statelessComponent("Note");

let make = (~noteInfo: Route.noteRouteConfig, _children) => {
  ...component,
  render: _self => {
    let noteQuery =
      GetNote.make(~noteId=noteInfo.noteId, ~username=noteInfo.username, ());
    <GetNoteQuery variables=noteQuery##variables>
      ...(
           ({result}) =>
             switch (result) {
             | Loading => <div> (ReasonReact.string("Loading")) </div>
             | Error(error) =>
               <div> (ReasonReact.string(error##message)) </div>
             | Data(response) =>
               let notes = response##note;
               switch (Array.length(notes)) {
               | 0 => <NotFound entity="note" />
               | _ =>
                 let note = notes[0];
                 <div> (note##id =>> (id => str(id))) </div>;
               };
             }
         )
    </GetNoteQuery>;
  },
};
