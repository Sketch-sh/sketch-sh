open Utils;
open Editor_Types;

module ForkButton = {
  let component =
    ReasonReact.statelessComponent("Editor_Note_SaveButton_Button");

  let make =
      (~hasSavePermission, ~handleFork, ~forkStatus, ~className=?, _children) => {
    ...component,
    render: _self =>
      <UI_Balloon position=Down message="Fork this Sketch">
        ...<button
             ariaLabel="Fork"
             disabled={
               switch (forkStatus) {
               | ForkStatus_Loading => true
               | _ => false
               }
             }
             ?className
             onClick={_ => {
               let continue =
                 if (hasSavePermission) {
                   Webapi.Dom.(
                     Window.confirm(
                       "Do you want to fork your own Sketch?",
                       window,
                     )
                   );
                 } else {
                   true;
                 };
               if (continue) {
                 handleFork();
               };
             }}>
             <UI_LoadingWrapper loading={forkStatus == ForkStatus_Loading}>
               ...{loading =>
                 loading
                   ? <Fi.Loader className="EditorNav__button--spin" />
                   : <Fi.GitBranch />
               }
             </UI_LoadingWrapper>
             <span> "Fork"->str </span>
           </button>
      </UI_Balloon>,
  };
};

module ForkLogin = {
  module ForkNoteLoginGql = [%graphql
    {|
    mutation (
      $noteId: String!,
      $userId: String!,
      $title: String!,
      $data: jsonb!,
      $forkFrom: String!,
    ) {
      mutate: insert_note(objects: [{
        title: $title,
        id: $noteId,
        user_id: $userId,
        data: $data,
        fork_from: $forkFrom
      }]) {
        returning {
          updated_at
          user_id
        }
      }
    }
  |}
  ];

  module ForkNoteLoginComponent =
    ReasonApollo.CreateMutation(ForkNoteLoginGql);

  let component =
    ReasonReact.statelessComponent("Editor_Note_SaveButton_Create");

  let make = (~noteId, ~userId, ~getCurrentData, ~updateForkStatus, children) => {
    ...component,
    render: _self =>
      <ForkNoteLoginComponent>
        ...{(mutation, _) =>
          children(~handleFork=() => {
            let (title, data) = getCurrentData();
            let newNoteId = Utils.generateId();

            let newNote =
              ForkNoteLoginGql.make(
                ~noteId=newNoteId,
                ~userId,
                ~title,
                ~data,
                ~forkFrom=noteId,
                (),
              );
            updateForkStatus(ForkStatus_Loading);
            Js.Promise.(
              mutation(~variables=newNote##variables, ())
              |> then_(
                   (
                     executionResponse:
                       ReasonApolloTypes.executionResponse(
                         ForkNoteLoginGql.t,
                       ),
                   ) => {
                   switch (executionResponse) {
                   | Data(data) =>
                     let newStatus =
                       switch (data##mutate) {
                       | None => ForkStatus_Error
                       | Some(mutate) =>
                         if (mutate##returning |> Array.length > 0) {
                           let newNoteData = mutate##returning[0];
                           ForkStatus_Done({
                             newId: newNoteId,
                             forkFrom: noteId,
                             lastEdited: newNoteData##updated_at,
                             owner: newNoteData##user_id,
                           });
                         } else {
                           ForkStatus_Error;
                         }
                       };
                     newStatus->updateForkStatus;
                   | EmptyResponse
                   | Errors(_) => ()
                   };
                   resolve();
                 })
              |> catch(err => {
                   updateForkStatus(ForkStatus_Error);
                   logError(err)->resolve;
                 })
              |> ignore
            );
          })
        }
      </ForkNoteLoginComponent>,
  };
};

module ForkAnonymous = {
  module ForkNoteAnonymousGql = [%graphql
    {|
    mutation forkNoteAnonymous (
      $noteId: String!,
      $editToken: String!,
      $userId: String!,
      $title: String!,
      $data: jsonb!,
      $forkFrom: String!,
    ) {
      mutate: insert_note(objects: [{
        title: $title,
        id: $noteId,
        user_id: $userId,
        data: $data,
        fork_from: $forkFrom
      }]) {
        returning {
          updated_at
          user_id
        }
      }
      insert_note_edit_token(objects:[{
        note_id: $noteId
        token: $editToken
      }]) {
        affected_rows
      }
    }
  |}
  ];

  module ForkNoteAnonymousComponent =
    ReasonApollo.CreateMutation(ForkNoteAnonymousGql);

  let component =
    ReasonReact.statelessComponent("Editor_Note_SaveButton_Create");

  let make = (~noteId, ~userId, ~getCurrentData, ~updateForkStatus, children) => {
    ...component,
    render: _self =>
      <ForkNoteAnonymousComponent>
        ...{(mutation, _createNoteResult) =>
          children(~handleFork=() => {
            let (title, data) = getCurrentData();
            let newNoteId = Utils.generateId();
            let newNote =
              ForkNoteAnonymousGql.make(
                ~noteId=newNoteId,
                ~userId,
                ~title,
                ~data,
                ~editToken=Auth.Auth.getOrCreateEditToken(),
                ~forkFrom=noteId,
                (),
              );
            updateForkStatus(ForkStatus_Loading);
            Js.Promise.(
              mutation(~variables=newNote##variables, ())
              |> then_(
                   (
                     executionResponse:
                       ReasonApolloTypes.executionResponse(
                         ForkNoteAnonymousGql.t,
                       ),
                   ) => {
                   switch (executionResponse) {
                   | Data(data) =>
                     let newStatus =
                       switch (data##mutate) {
                       | None => ForkStatus_Error
                       | Some(mutate) =>
                         if (mutate##returning |> Array.length > 0) {
                           let newNoteData = mutate##returning[0];
                           ForkStatus_Done({
                             newId: newNoteId,
                             forkFrom: noteId,
                             lastEdited: newNoteData##updated_at,
                             owner: newNoteData##user_id,
                           });
                         } else {
                           ForkStatus_Error;
                         }
                       };
                     newStatus->updateForkStatus;
                   | EmptyResponse
                   | Errors(_) => ()
                   };
                   resolve();
                 })
              |> catch(err => {
                   updateForkStatus(ForkStatus_Error);
                   logError(err)->resolve;
                 })
              |> ignore
            );
          })
        }
      </ForkNoteAnonymousComponent>,
  };
};

let component = ReasonReact.statelessComponent("Editor_Note_SaveButton");

let make =
    (
      ~hasSavePermission,
      ~forkStatus,
      ~noteId,
      ~noteState,
      ~getCurrentData,
      ~updateForkStatus,
      ~className=?,
      _children,
    ) => {
  ...component,
  render: _self =>
    switch (noteState) {
    | NoteState_New => React.null
    | NoteState_Old =>
      <AuthStatus.IsAuthenticated>
        ...(
             fun
             | Login(userId) =>
               <ForkLogin getCurrentData userId noteId updateForkStatus>
                 ...{(~handleFork) =>
                   <ForkButton
                     hasSavePermission
                     forkStatus
                     handleFork
                     ?className
                   />
                 }
               </ForkLogin>
             | Anonymous =>
               <ForkAnonymous
                 getCurrentData
                 userId=Config.anonymousUserId
                 noteId
                 updateForkStatus>
                 ...{(~handleFork) =>
                   <ForkButton
                     hasSavePermission
                     forkStatus
                     handleFork
                     ?className
                   />
                 }
               </ForkAnonymous>
           )
      </AuthStatus.IsAuthenticated>
    },
};
