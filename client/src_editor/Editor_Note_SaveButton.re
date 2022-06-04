open Utils;
open Editor_Types;

module SaveButton = {
  type action =
    | Save;

  let component =
    ReasonReact.reducerComponent("Editor_Note_SaveButton_Button");

  let make =
      (
        ~hasSavePermission,
        ~handleSave,
        ~editorContentStatus,
        ~className=?,
        ~registerShortcut: Shortcut.subscribeFun,
        _children,
      )
      : React.component(unit, 'a, action) => {
    ...component,
    didMount: ({send, onUnmount}) =>
      registerShortcut(
        ~global=true,
        "mod+s",
        event => {
          Webapi.Dom.(event->KeyboardEvent.preventDefault);
          send(Save);
        },
      )
      ->onUnmount,
    reducer: (action, _state) =>
      switch (action) {
      | Save =>
        if (!hasSavePermission) {
          Notify.error(
            "You don't have permission to update this Sketch. You can fork it instead",
          );
          ReasonReact.NoUpdate;
        } else {
          /* TODO: Don't save when there is nothing to do */
          ReasonReact.SideEffects(
            _ => handleSave(),
          );
        }
      },
    render: self =>
      <UI_Balloon
        position=Down
        length=Fit
        message={
          switch (editorContentStatus) {
          | Ec_Saving => "Saving...(Ctrl+S)"
          | Ec_Saved => "Saved (Ctrl+S)"
          | Ec_Pristine => "Nothing to save (Ctrl+S)"
          | Ec_Dirty => "Save modified changes (Ctrl+S)"
          }
        }>
        ...<button
             disabled={
               switch (editorContentStatus) {
               | Ec_Saving
               | Ec_Saved
               | Ec_Pristine => true
               | Ec_Dirty => false
               }
             }
             ?className
             onClick={_ => self.send(Save)}>
             <UI_LoadingWrapper loading={editorContentStatus == Ec_Saving}>
               ...{loading =>
                 loading
                   ? <Fi.Loader className="EditorNav__button--spin" />
                   : <Fi.Save />
               }
             </UI_LoadingWrapper>
             <span> "Save"->str </span>
           </button>
      </UI_Balloon>,
  };
};

module CreateLogin = {
  module CreateLoginGql = [%graphql
    {|
    mutation (
      $noteId: String!,
      $userId: String!,
      $title: String!,
      $compilerVersion: String!,
      $data: jsonb!
    ) {
      mutate: insert_note(objects: [{
        title: $title,
        id: $noteId,
        user_id: $userId,
        compiler_version: $compilerVersion,
        data: $data
      }]) {
        returning {
          updated_at
        }
      }
    }
  |}
  ];

  module CreateLoginComponent = ReasonApollo.CreateMutation(CreateLoginGql);

  let component =
    ReasonReact.statelessComponent("Editor_Note_SaveButton_Create");

  let make =
      (
        ~noteId,
        ~compilerVersion,
        ~userId,
        ~getCurrentData,
        ~updateSaveStatus,
        children,
      ) => {
    ...component,
    render: _self =>
      <CreateLoginComponent>
        ...{(mutation, _) =>
          children(~handleSave=() => {
            open Js.Promise;
            let (title, data) = getCurrentData();
            let compilerVersion =
              CompilerVersion.(compilerVersion->toDbString);
            let newNote =
              CreateLoginGql.make(
                ~noteId,
                ~compilerVersion,
                ~userId,
                ~title,
                ~data,
                (),
              );
            updateSaveStatus(SaveStatus_Loading);

            mutation(~variables=newNote##variables, ())
            |> then_(
                 (
                   executionResponse:
                     ReasonApolloTypes.executionResponse(CreateLoginGql.t),
                 ) => {
                 switch (executionResponse) {
                 | Data(data) =>
                   let newSaveStatus =
                     switch (data##mutate) {
                     | None => SaveStatus_Error
                     | Some(mutate) =>
                       if (mutate##returning |> Array.length > 0) {
                         let newNoteData = mutate##returning[0];
                         SaveStatus_Done({
                           lastEdited: newNoteData##updated_at,
                         });
                       } else {
                         SaveStatus_Error;
                       }
                     };
                   newSaveStatus->updateSaveStatus;
                 | EmptyResponse
                 | Errors(_) => ()
                 };
                 resolve();
               })
            |> catch(err => {
                 updateSaveStatus(SaveStatus_Error);
                 logError(err)->resolve;
               })
            |> ignore;
          })
        }
      </CreateLoginComponent>,
  };
};

module CreateAnonymous = {
  module CreateNoteAnonymousGql = [%graphql
    {|
    mutation createNoteAnonymous (
      $noteId: String!,
      $editToken: String!,
      $userId: String!,
      $title: String!,
      $compilerVersion: String!,
      $data: jsonb!
    ) {
      mutate: insert_note(objects: [{
        title: $title,
        id: $noteId,
        user_id: $userId,
        compiler_version: $compilerVersion,
        data: $data
      }]) {
        returning {
          updated_at
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

  module CreateNoteAnonymousComponent =
    ReasonApollo.CreateMutation(CreateNoteAnonymousGql);

  let component =
    ReasonReact.statelessComponent("Editor_Note_SaveButton_Create");

  let make = (~noteId, ~userId, ~getCurrentData, ~updateSaveStatus, children) => {
    ...component,
    render: _self =>
      <CreateNoteAnonymousComponent>
        ...{(mutation, _) =>
          children(~handleSave=() => {
            open Js.Promise;
            let (title, data) = getCurrentData();
            let compilerVersion = CompilerVersion.(current->toDbString);
            let newNote =
              CreateNoteAnonymousGql.make(
                ~noteId,
                ~userId,
                ~title,
                ~data,
                ~compilerVersion,
                ~editToken=Auth.Auth.getOrCreateEditToken(),
                (),
              );
            updateSaveStatus(SaveStatus_Loading);

            mutation(~variables=newNote##variables, ())
            |> then_(
                 (
                   executionResponse:
                     ReasonApolloTypes.executionResponse(
                       CreateNoteAnonymousGql.t,
                     ),
                 ) => {
                 switch (executionResponse) {
                 | Data(data) =>
                   let newSaveStatus =
                     switch (data##mutate) {
                     | None => SaveStatus_Error
                     | Some(mutate) =>
                       if (mutate##returning |> Array.length > 0) {
                         let newNoteData = mutate##returning[0];
                         SaveStatus_Done({
                           lastEdited: newNoteData##updated_at,
                         });
                       } else {
                         SaveStatus_Error;
                       }
                     };
                   newSaveStatus->updateSaveStatus;
                 | EmptyResponse
                 | Errors(_) => ()
                 };
                 resolve();
               })
            |> catch(err => {
                 updateSaveStatus(SaveStatus_Error);
                 logError(err)->resolve;
               })
            |> ignore;
          })
        }
      </CreateNoteAnonymousComponent>,
  };
};

module Update = {
  open GqlUpdateNote;

  let component =
    ReasonReact.statelessComponent("Editor_Note_SaveButton_Update");

  let make = (~noteId, ~getCurrentData, ~updateSaveStatus, children) => {
    ...component,
    render: _self =>
      <UpdateNoteComponent>
        ...{(mutation, _) =>
          children(~handleSave=() => {
            open Js.Promise;
            let (title, data) = getCurrentData();
            let updateNote = UpdateNoteGql.make(~noteId, ~title, ~data, ());
            updateSaveStatus(SaveStatus_Loading);

            mutation(~variables=updateNote##variables, ())
            |> then_(
                 (
                   executionResponse:
                     ReasonApolloTypes.executionResponse(UpdateNoteGql.t),
                 ) => {
                 switch (executionResponse) {
                 | Data(data) =>
                   let newSaveStatus =
                     switch (data##mutate) {
                     | None => SaveStatus_Error
                     | Some(mutate) =>
                       if (mutate##returning |> Array.length > 0) {
                         let newNoteData = mutate##returning[0];
                         SaveStatus_Done({
                           lastEdited: newNoteData##updated_at,
                         });
                       } else {
                         SaveStatus_Error;
                       }
                     };
                   newSaveStatus->updateSaveStatus;
                 | EmptyResponse
                 | Errors(_) => ()
                 };
                 resolve();
               })
            |> catch(err => {
                 updateSaveStatus(SaveStatus_Error);
                 logError(err)->resolve;
               })
            |> ignore;
          })
        }
      </UpdateNoteComponent>,
  };
};

let component = ReasonReact.statelessComponent("Editor_Note_SaveButton");

let make =
    (
      ~hasSavePermission,
      ~noteId,
      ~compilerVersion,
      ~getCurrentData,
      ~noteState,
      ~updateSaveStatus,
      ~className=?,
      ~editorContentStatus,
      ~registerShortcut,
      _children,
    ) => {
  ...component,
  render: _self =>
    <AuthStatus.IsAuthenticated>
      ...{user =>
        switch (user, noteState) {
        | (Login(userId), NoteState_New) =>
          <CreateLogin
            getCurrentData userId noteId compilerVersion updateSaveStatus>
            ...{(~handleSave) =>
              <SaveButton
                hasSavePermission
                editorContentStatus
                handleSave
                ?className
                registerShortcut
              />
            }
          </CreateLogin>
        | (Anonymous, NoteState_Old)
        | (Login(_), NoteState_Old) =>
          <Update getCurrentData noteId updateSaveStatus>
            ...{(~handleSave) =>
              <SaveButton
                hasSavePermission
                editorContentStatus
                handleSave
                ?className
                registerShortcut
              />
            }
          </Update>
        | (Anonymous, NoteState_New) =>
          <CreateAnonymous
            getCurrentData
            userId=Config.anonymousUserId
            noteId
            updateSaveStatus>
            ...{(~handleSave) =>
              <SaveButton
                hasSavePermission
                editorContentStatus
                handleSave
                ?className
                registerShortcut
              />
            }
          </CreateAnonymous>
        }
      }
    </AuthStatus.IsAuthenticated>,
};
