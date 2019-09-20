open GqlGetNoteById;
open Utils;
open Editor_Types;

/*
 * This module will ensure url encoded data always in the URL
 * on initial note load
 */
module RedirectSketchURL = {
  let component = ReasonReact.reducerComponent("Note_RedirectSketchURL");

  let make = (~noteId, children): ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: _ => Router.replaceSilent(Route.Note({noteId, data: None})),
    render: _send => children,
  };
};

let component = ReasonReact.statelessComponent("Note");

[@react.component]
let make =
    (~noteInfo: Route.noteRouteConfig, _children: ReactHelpers.childless) =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self => {
      let noteId = noteInfo.noteId;
      let noteQuery = GetNoteById.make(~noteId, ());
      let%Epitath authStatus = children =>
        <AuthStatus.IsAuthenticated> ...children </AuthStatus.IsAuthenticated>;

      let%Epitath {result} =
        children =>
          <GetNoteByIdComponent variables=noteQuery##variables>
            ...children
          </GetNoteByIdComponent>;

      switch (result) {
      | Loading => <Editor_NotePlaceholder />
      | Error(error) => error##message->str
      | Data(response) =>
        let notes = response##note;
        notes->(
                 arrayFirst(
                   ~empty=<NotFound entity="note" />,
                   ~render=note => {
                     let (lang, links, blocks) =
                       switch (note##data) {
                       | None => (Editor_Types.RE, [||], [||])
                       | Some(data) => data->Editor_Json.V1.decode
                       };
                     let hasSavePermission =
                       switch (authStatus) {
                       | Login(currentUserId) =>
                         note##user_id == currentUserId
                       | Anonymous =>
                         response##note_edit_token->Array.length > 0
                       };

                     let blocks =
                       Editor_Blocks_Utils.filterDeletedBlocks(blocks);

                     <RedirectSketchURL noteId>
                       ...<Editor_Note
                            key=noteId
                            noteOwnerId=note##user_id
                            noteLastEdited={Some(note##updated_at)}
                            noteId
                            noteState=NoteState_Old
                            title=?{note##title}
                            lang
                            links
                            blocks
                            forkFrom=?{note##fork_from}
                            hasSavePermission
                          />
                     </RedirectSketchURL>;
                   },
                 )
               );
      };
    },
  });
