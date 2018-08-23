open Utils;
open Editor_Types;

module ForkButton = {
  let component =
    ReasonReact.statelessComponent("Editor_Note_SaveButton_Button");

  let make = (~handleFork, ~forkStatus, ~className=?, _children) => {
    ...component,
    render: _self =>
      <UI_Balloon position=Down message="Fork this Sketch">
        ...<button
             disabled=(
               switch (forkStatus) {
               | ForkStatus_Loading => true
               | _ => false
               }
             )
             ?className
             onClick=(_ => handleFork())>
             (
               switch (forkStatus) {
               | ForkStatus_Loading =>
                 <>
                   <Fi.Loader className="EditorNav__button--spin" />
                   "Forking"->str
                 </>
               | _ => <> <Fi.Save /> "Fork"->str </>
               }
             )
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
      mutate: insert_note(objects: {
        title: $title,
        id: $noteId,
        user_id: $userId,
        data: $data,
        fork_from: $forkFrom
      }) {
        returning {
          updated_at
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
        ...(
             (mutation, _) =>
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
                   |> then_(apolloData => {
                        switch (apolloData##data |> Js.Nullable.toOption) {
                        | None => ()
                        | Some(data) =>
                          let data = ForkNoteLoginGql.parse(data);
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
                                });
                              } else {
                                ForkStatus_Error;
                              }
                            };
                          newStatus->updateForkStatus;
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
           )
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
      mutate: insert_note(objects: {
        title: $title,
        id: $noteId,
        user_id: $userId,
        data: $data,
        fork_from: $forkFrom
      }) {
        returning {
          updated_at
        }
      }
      insert_note_edit_token(objects:{
        note_id: $noteId
        token: $editToken
      }) {
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
        ...(
             (mutation, _createNoteResult) =>
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
                   |> then_(apolloData => {
                        switch (apolloData##data |> Js.Nullable.toOption) {
                        | None => ()
                        | Some(data) =>
                          let data = ForkNoteAnonymousGql.parse(data);
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
                                });
                              } else {
                                ForkStatus_Error;
                              }
                            };
                          newStatus->updateForkStatus;
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
           )
      </ForkNoteAnonymousComponent>,
  };
};

let component = ReasonReact.statelessComponent("Editor_Note_SaveButton");

let make =
    (
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
                 ...(
                      (~handleFork) =>
                        <ForkButton forkStatus handleFork ?className />
                    )
               </ForkLogin>
             | Anonymous =>
               <ForkAnonymous
                 getCurrentData
                 userId=Config.anonymousUserId
                 noteId
                 updateForkStatus>
                 ...(
                      (~handleFork) =>
                        <ForkButton forkStatus handleFork ?className />
                    )
               </ForkAnonymous>
           )
      </AuthStatus.IsAuthenticated>
    },
};
