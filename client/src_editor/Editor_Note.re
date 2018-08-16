[%%debugger.chrome];
Modules.require("./Editor_Note.css");
open Utils;
open Editor_Types.Block;

module Editor_Note = {
  type editorContentStatus =
    | Ec_Pristine
    | Ec_Dirty
    | Ec_Saving
    | Ec_Saved;

  type state = {
    title: string,
    editorContentStatus,
    noteSaveStatus: ref(NoteSave_Types.noteSaveStatus),
    blocks: ref(array(block)),
    executeCallback: option(unit => unit),
  };

  type action =
    | TitleUpdate(string)
    | BlockUpdate(array(block))
    | RegisterExecuteCallback(unit => unit)
    | Save;

  let component = ReasonReact.reducerComponent("Editor_Page");

  let make =
      (
        ~blocks,
        ~noteOwner=?,
        ~noteLastEdited=?,
        ~title="",
        ~noteSaveStatus: NoteSave_Types.noteSaveStatus,
        ~isEditable,
        ~onSave,
        ~registerShortcut: Shortcut.subscribeFun,
        _children,
      ) => {
    ...component,
    initialState: () => {
      title,
      editorContentStatus: Ec_Pristine,
      noteSaveStatus: ref(noteSaveStatus),
      blocks: ref(blocks),
      executeCallback: None,
    },
    willReceiveProps: ({state}) =>
      if (state.noteSaveStatus^ != noteSaveStatus) {
        state.noteSaveStatus := noteSaveStatus;
        switch (state.editorContentStatus, noteSaveStatus) {
        | (_, NoteSave_Loading) => {...state, editorContentStatus: Ec_Saving}
        | (Ec_Dirty, NoteSave_Done) => {
            ...state,
            editorContentStatus: Ec_Dirty,
          }
        | (_, NoteSave_Done) => {...state, editorContentStatus: Ec_Saved}
        | (_, NoteSave_Error) =>
          UI_Notification.notify("Save error");
          {...state, editorContentStatus: Ec_Dirty};
        };
      } else {
        state;
      },
    didUpdate: ({oldSelf, newSelf}) =>
      if (newSelf.state.editorContentStatus
          != oldSelf.state.editorContentStatus) {
        let unloadMessage =
          switch (newSelf.state.editorContentStatus) {
          | Ec_Dirty => Some("Changes you made may not be saved")
          | _ => None
          };
        Router.Unload.register(unloadMessage);
      },
    didMount: ({send, onUnmount}) => {
      let unregisterShortcut =
        registerShortcut(
          ~global=true,
          "mod+s",
          event => {
            open Webapi.Dom;

            event->KeyboardEvent.preventDefault;
            send(Save);
          },
        );

      onUnmount(() => {
        Router.Unload.unregister();
        unregisterShortcut();
      });
    },
    reducer: (action, state) =>
      switch (action) {
      | TitleUpdate(title) =>
        ReasonReact.Update({...state, title, editorContentStatus: Ec_Dirty})
      | RegisterExecuteCallback(callback) =>
        ReasonReact.Update({...state, executeCallback: Some(callback)})
      | BlockUpdate(blocks) =>
        state.blocks := blocks;
        ReasonReact.Update({...state, editorContentStatus: Ec_Dirty});
      | Save =>
        /* TODO: nice notification */
        switch (state.editorContentStatus) {
        | Ec_Saving
        | Ec_Saved
        | Ec_Pristine => ()
        | Ec_Dirty => onSave(~title=state.title, ~data=state.blocks^)
        };
        ReasonReact.NoUpdate;
      },
    render: ({state, send}) => {
      let readOnly = !isEditable;
      let {editorContentStatus} = state;

      <>
        <UI_Topbar.Actions>
          ...(
               (~buttonClassName) =>
                 <>
                   <UI_Balloon position=Down message="Not implemented">
                     ...<button className=buttonClassName disabled=true>
                          <Fi.GitBranch />
                          "Fork"->str
                        </button>
                   </UI_Balloon>
                   <UI_Balloon
                     position=Down
                     length=Fit
                     message="Execute code (Shift+Enter)">
                     ...<button
                          className=buttonClassName
                          onClick=(
                            _ =>
                              switch (state.executeCallback) {
                              | None => ()
                              | Some(callback) => callback()
                              }
                          )>
                          <Fi.Terminal />
                          "Run"->str
                        </button>
                   </UI_Balloon>
                   (
                     readOnly ?
                       ReasonReact.null :
                       <UI_Balloon
                         position=Down
                         length=Fit
                         message=(
                           switch (editorContentStatus) {
                           | Ec_Saving => "Saving...(Ctrl+S)"
                           | Ec_Saved => "Saved (Ctrl+S)"
                           | Ec_Pristine => "Nothing to save (Ctrl+S)"
                           | Ec_Dirty => "Save modified changes (Ctrl+S)"
                           }
                         )>
                         ...<button
                              disabled=(
                                switch (editorContentStatus) {
                                | Ec_Saving
                                | Ec_Saved
                                | Ec_Pristine => true
                                | Ec_Dirty => false
                                }
                              )
                              className=buttonClassName
                              onClick=(
                                _ => {
                                  onSave(
                                    ~title=state.title,
                                    ~data=state.blocks^,
                                  );
                                  switch (state.executeCallback) {
                                  | None => ()
                                  | Some(callback) => callback()
                                  };
                                }
                              )>
                              (
                                switch (editorContentStatus) {
                                | Ec_Saving =>
                                  <>
                                    <Fi.Loader
                                      className="EditorNav__button--spin"
                                    />
                                    "Saving"->str
                                  </>
                                | Ec_Saved
                                | Ec_Dirty
                                | Ec_Pristine => <> <Fi.Save /> "Save"->str </>
                                }
                              )
                            </button>
                       </UI_Balloon>
                   )
                 </>
             )
        </UI_Topbar.Actions>
        <div className="EditorNote__saveStatus">
          {
            let status =
              switch (editorContentStatus) {
              | Ec_Pristine => ""
              | Ec_Saved => "Saved"
              | Ec_Saving => "Saving"
              | Ec_Dirty => "Unsaved"
              };
            status->str;
          }
        </div>
        <main className="EditorNote Layout__center">
          <Helmet>
            <title>
              {
                let title = state.title == "" ? "untitled" : state.title;
                title->str;
              }
            </title>
          </Helmet>
          <div className="EditorNote__metadata">
            <input
              className="EditorNote__metadata--title"
              placeholder="untitled sketch"
              value=state.title
              onChange=(event => valueFromEvent(event)->TitleUpdate->send)
              readOnly
            />
            (
              noteOwner
              =>> (
                noteOwner => <Editor_NoteOwnerInfo noteOwner ?noteLastEdited />
              )
            )
          </div>
          <Editor_Blocks
            blocks
            registerExecuteCallback=(
              callback => send(RegisterExecuteCallback(callback))
            )
            onUpdate=(blocks => send(BlockUpdate(blocks)))
            readOnly
          />
        </main>
      </>;
    },
  };
};

module WithShortcut = {
  let component = ReasonReact.statelessComponent("WithShortcut(Editor_Note)");

  let make =
      (
        ~blocks,
        ~noteOwner=?,
        ~noteLastEdited=?,
        ~title=?,
        ~noteSaveStatus,
        ~isEditable,
        ~onSave,
        _children,
      ) => {
    ...component,
    render: _self =>
      <Shortcut.Consumer>
        ...(
             registerShortcut =>
               <Editor_Note
                 blocks
                 ?noteOwner
                 ?noteLastEdited
                 ?title
                 noteSaveStatus
                 isEditable
                 onSave
                 registerShortcut
               />
           )
      </Shortcut.Consumer>,
  };
};

include WithShortcut;
