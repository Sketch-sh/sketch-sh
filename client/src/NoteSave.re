/*
 * See https://github.com/thangngoc89/rtop-ui/issues/76
 * Use pushState when saving newNote
 *     replaceState when adjusting oldNote URL
 */
type pushOrReplace =
  | Push
  | Replace;

let replaceNoteRoute = (~noteId, ~json, ~title, ~kind) =>
  Js.Promise.(
    LzString.async()
    |> then_(lzstring =>
         (
           lzstring->(LzString.URI.compress(title))
           ++ "---"
           ++ lzstring->LzString.URI.compress(Js.Json.stringify(json))
         )
         ->resolve
       )
    |> then_(compressed => {
         let routerAction =
           switch (kind) {
           | Push => Router.pushSilent
           | Replace => Router.replaceSilent
           };
         routerAction(Route.Note({noteId, data: Some(compressed)}))
         ->resolve;
       })
    |> Utils.handleError
  );

open NoteSave_Types;
type action =
  | SavedNewNote(id, string, Js.Json.t)
  | SavedOldNote(id, string, Js.Json.t);

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
    | SavedNewNote(noteId, title, json) => ReasonReact.NoUpdate
    /* ReasonReact.UpdateWithSideEffects(
         {kind: Old(noteId)},
         (
           _self => replaceNoteRoute(~noteId, ~json, ~title, ~kind=Push)->ignore
         ),
       ) */
    | SavedOldNote(noteId, title, json) => ReasonReact.NoUpdate
    /* ReasonReact.SideEffects(
         (
           _self =>
             replaceNoteRoute(~noteId, ~json, ~title, ~kind=Replace)->ignore
         ),
       ) */
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
                   (noteId, title, data) =>
                     SavedNewNote(noteId, title, data)->send
                 )
                 onSaveOldNote=(
                   (noteId, title, data) =>
                     SavedOldNote(noteId, title, data)->send
                 )>
                 ...children
               </NoteSave_Anonymous>
             | Login(userId) =>
               <NoteSave_Login
                 kind=state.kind
                 userId
                 onSaveNewNote=(
                   (noteId, title, data) =>
                     SavedNewNote(noteId, title, data)->send
                 )
                 onSaveOldNote=(
                   (noteId, title, data) =>
                     SavedOldNote(noteId, title, data)->send
                 )>
                 ...children
               </NoteSave_Login>
             }
         )
    </AuthStatus.IsAuthenticated>,
};
