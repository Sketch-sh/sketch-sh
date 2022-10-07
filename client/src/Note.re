open GqlGetNoteById;
open Utils;
open Editor_Types;

/*
 * This module will ensure url encoded data always in the URL
 * on initial note load
 */
module RedirectSketchURL = {
  [@react.component]
  let make = (~noteId, ~children) => {
    React.useEffect0(() => {
      Router.replaceSilent(Route.Note({noteId, data: None}));
      None;
    });

    children;
  };
};

[@react.component]
let make = (~noteInfo: Route.noteRouteConfig) => {
  let noteId = noteInfo.noteId;
  let noteQuery = GetNoteById.make(~noteId, ());

  <GetNoteByIdComponent variables=noteQuery##variables>
    ...{({refetch, result}) => {
    let refetch: option(unit => Js.Promise.t('g)), "title": option(string) = Some(
                                    () =>
                                      refetch(Some(noteQuery##variables)),
                                  );
      switch (result) {
      | Loading => <Editor_NotePlaceholder />
      | Error(error) => error.message->str
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
                     <AuthStatus.IsAuthenticated>
                       ...{authStatus => {
                         let hasSavePermission =
                           switch (authStatus) {
                           | AuthStatus.Login(currentUserId) =>
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
                                compilerVersion={
                                  note##compiler_version
                                  ->CompilerVersion.ofDbString
                                }
                                title=?{note##title}
                                lang
                                links
                                blocks
                                forkFrom=?{note##fork_from}
                                hasSavePermission
                                refetch=refetch
                              />
                         </RedirectSketchURL>;
                       }}
                     </AuthStatus.IsAuthenticated>;
                   },
                 )
               );
      }}
    }
  </GetNoteByIdComponent>;
};
