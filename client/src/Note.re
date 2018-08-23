module GetNote = [%graphql
  {|
    query getNote (
      $noteId: String!
    ) {
      note: note (where: {id : {_eq: $noteId}}) {
        ...GqlFragment.Editor.EditorNote
      }
      note_edit_token(where: {note_id: {_eq: $noteId}}) {
        note_id
      }
    }
  |}
];

module GetNoteComponent = ReasonApollo.CreateQuery(GetNote);

open Utils;
open Editor_Types;

/*
 * This module will ensure url encoded data always in the URL
 * on initial note load
 */
module RedirectSketchURL = {
  let component = ReasonReact.reducerComponent("Note_RedirectSketchURL");

  let make = (~noteId, children): React.component(unit, 'a, unit) => {
    ...component,
    didMount: _ => Router.replaceSilent(Route.Note({noteId, data: None})),
    render: _send => children,
  };
};

let component = ReasonReact.statelessComponent("Note");

let make = (~noteInfo: Route.noteRouteConfig, _children: React.childless) => {
  ...component,
  render: _self => {
    let noteId = noteInfo.noteId;
    let noteQuery = GetNote.make(~noteId, ());

    <GetNoteComponent variables=noteQuery##variables>
      ...(
           ({result}) =>
             switch (result) {
             | Loading => <Editor_NotePlaceholder />
             | Error(error) => error##message->str
             | Data(response) =>
               let notes = response##note;
               notes
               ->(
                   arrayFirst(
                     ~empty=<NotFound entity="note" />,
                     ~render=note => {
                       let (lang, blocks) =
                         switch (note##data) {
                         | None => (Editor_Types.RE, [||])
                         | Some(blocks) => blocks->Editor_Json.V1.decode
                         };
                       <RedirectSketchURL noteId>
                         ...<Editor_Note
                              noteOwnerId=note##user_id
                              noteLastEdited=(Some(note##updated_at))
                              noteId
                              noteState=NoteState_Old
                              title=?(note##title)
                              lang
                              blocks
                            />
                       </RedirectSketchURL>;
                     },
                   )
                 );
             }
         )
    </GetNoteComponent>;
  },
};
