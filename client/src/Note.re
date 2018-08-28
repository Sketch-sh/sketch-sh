open GqlGetNoteById;
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
    let noteQuery = GetNoteById.make(~noteId, ());
    <AuthStatus.IsAuthenticated>
      ...(
           user =>
             <GetNoteByIdComponent variables=noteQuery##variables>
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
                                  | Some(blocks) =>
                                    blocks->Editor_Json.V1.decode
                                  };
                                let hasSavePermission =
                                  switch (user) {
                                  | Login(currentUserId) =>
                                    note##user_id == currentUserId
                                  | Anonymous =>
                                    response##note_edit_token->Array.length > 0
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
                                       forkFrom=?(note##fork_from)
                                       hasSavePermission
                                     />
                                </RedirectSketchURL>;
                              },
                            )
                          );
                      }
                  )
             </GetNoteByIdComponent>
         )
    </AuthStatus.IsAuthenticated>;
  },
};
