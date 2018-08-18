/*
 * See https://github.com/thangngoc89/rtop-ui/issues/76
 * Use pushState when saving newNote
 *     replaceState when adjusting oldNote URL
 */
type pushOrReplace =
  | Push
  | Replace;

let replaceNoteRoute = (~noteId, ~kind) => {
  let routerAction =
    switch (kind) {
    | Push => Router.pushSilent
    | Replace => Router.replaceSilent
    };
  routerAction(Route.Note({noteId, data: None}));
};

open NoteSave_Types;
type action =
  | SavedNewNote(id)
  | SavedOldNote(id);

/* TODO:
   When receive the mutation result,
   check with the original data to see if they are matched */
let component = ReasonReact.reducerComponent("NoteSave");

let make =
    (
      ~noteKind: noteKind,
      children:
        (
          ~noteSaveStatus: NoteSave_Types.noteSaveStatus,
          ~user: AuthStatus.state,
          ~onSave: 'a
        ) =>
        ReasonReact.reactElement,
    ) => {
  ...component,
  initialState: () => {kind: noteKind},
  reducer: (action, _state) =>
    switch (action) {
    | SavedNewNote(noteId) =>
      ReasonReact.UpdateWithSideEffects(
        {kind: Old(noteId)},
        (_self => replaceNoteRoute(~noteId, ~kind=Push)->ignore),
      )
    | SavedOldNote(_noteId) => ReasonReact.NoUpdate
    },
  render: ({state, send}) =>
    <AuthStatus.IsAuthenticated>
      ...(
           user =>
             switch (user) {
             | Anonymous =>
               <NoteSave_Anonymous
                 kind=state.kind
                 onSaveNewNote=(
                   (noteId, _title, _data) => SavedNewNote(noteId)->send
                 )
                 onSaveOldNote=(
                   (noteId, _title, _data) => SavedOldNote(noteId)->send
                 )>
                 ...children
               </NoteSave_Anonymous>
             | Login(userId) =>
               <NoteSave_Login
                 kind=state.kind
                 userId
                 onSaveNewNote=(
                   (noteId, _title, _data) => SavedNewNote(noteId)->send
                 )
                 onSaveOldNote=(
                   (noteId, _title, _data) => SavedOldNote(noteId)->send
                 )>
                 ...children
               </NoteSave_Login>
             }
         )
    </AuthStatus.IsAuthenticated>,
};
