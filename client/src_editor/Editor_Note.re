[%%debugger.chrome];
Modules.require("./Editor_Note.css");
open Utils;
open Editor_Types;

module Editor_Note = {
  type state = {
    hasSavePermission: bool,
    noteId: id,
    forkFrom: option(id),
    noteState,
    lang,
    title: string,
    blocks: ref(array(Block.block)),
    editorContentStatus,
    executeCallback: option(unit => unit),
    noteOwnerId: id,
    noteLastEdited: option(Js.Json.t),
    forkStatus,
  };

  type action =
    | TitleUpdate(string)
    | BlockUpdate(array(Block.block))
    | RegisterExecuteCallback(unit => unit)
    | UpdateNoteSaveStatus(saveStatus)
    | UpdateForkStatus(forkStatus)
    | Execute
    | ChangeLang(lang);

  let component = ReasonReact.reducerComponent("Editor_Page");

  let make =
      (
        ~initialHasSavePermission,
        ~initialNoteId,
        ~initialForkFrom=?,
        ~initialNoteState: noteState,
        ~initialLang: lang=RE,
        ~initialTitle: string="",
        ~initialBlocks: array(Block.block),
        ~initialNoteOwnerId: id,
        ~initialNoteLastEdited: option(Js.Json.t),
        ~registerShortcut: Shortcut.subscribeFun,
        _children,
      ) => {
    let makeInitialState = () => {
      hasSavePermission: initialHasSavePermission,
      noteId: initialNoteId,
      forkFrom: initialForkFrom,
      noteState: initialNoteState,
      lang: initialLang,
      title: initialTitle,
      editorContentStatus: Ec_Pristine,
      blocks: ref(initialBlocks),
      executeCallback: None,
      noteOwnerId: initialNoteOwnerId,
      noteLastEdited: initialNoteLastEdited,
      forkStatus: ForkStatus_Initial,
    };
    {
      ...component,
      initialState: makeInitialState,
      willReceiveProps: self =>
        if (self.state.noteId != initialNoteId) {
          makeInitialState();
        } else {
          self.state;
        },
      didUpdate: ({oldSelf, newSelf}) =>
        if (newSelf.state.editorContentStatus
            != oldSelf.state.editorContentStatus) {
          let unloadMessage =
            switch (newSelf.state.editorContentStatus) {
            | Ec_Saved
            | Ec_Pristine => None
            | _ => Some("Changes you made may not be saved")
            };
          Router.Unload.register(unloadMessage);
          /* This execute the code after save */
          if (newSelf.state.editorContentStatus == Ec_Saved) {
            newSelf.send(Execute);
          };
        },
      didMount: ({onUnmount}) => onUnmount(Router.Unload.unregister),
      reducer: (action, state) =>
        switch (action) {
        | TitleUpdate(title) =>
          ReasonReact.Update({...state, title, editorContentStatus: Ec_Dirty})
        | RegisterExecuteCallback(callback) =>
          ReasonReact.Update({...state, executeCallback: Some(callback)})
        | Execute =>
          ReasonReact.SideEffects(
            (
              ({state}) =>
                switch (state.executeCallback) {
                | None => ()
                | Some(callback) => callback()
                }
            ),
          )
        | BlockUpdate(blocks) =>
          state.blocks := blocks;
          ReasonReact.Update({...state, editorContentStatus: Ec_Dirty});
        | ChangeLang(lang) =>
          ReasonReact.Update({...state, lang, editorContentStatus: Ec_Dirty})
        | UpdateNoteSaveStatus(saveStatus) =>
          switch (state.editorContentStatus, saveStatus) {
          | (_, SaveStatus_Initial) => ReasonReact.NoUpdate
          | (_, SaveStatus_Loading) =>
            ReasonReact.Update({...state, editorContentStatus: Ec_Saving})
          | (Ec_Dirty, SaveStatus_Done({lastEdited: _})) =>
            ReasonReact.Update({...state, editorContentStatus: Ec_Dirty})
          | (_, SaveStatus_Done({lastEdited})) =>
            Notify.info("Saved successfully");
            ReasonReact.UpdateWithSideEffects(
              {
                ...state,
                editorContentStatus: Ec_Saved,
                noteState: NoteState_Old,
                noteLastEdited: Some(lastEdited),
              },
              (
                _ =>
                  Router.pushSilent(
                    Route.Note({noteId: state.noteId, data: None}),
                  )
              ),
            );
          | (_, SaveStatus_Error) =>
            Notify.error("Save error");
            ReasonReact.Update({...state, editorContentStatus: Ec_Dirty});
          }
        | UpdateForkStatus(forkStatus) =>
          switch (forkStatus) {
          | ForkStatus_Done({newId, forkFrom, lastEdited, owner}) =>
            Notify.info("Forked successfully");
            ReasonReact.UpdateWithSideEffects(
              {
                ...state,
                forkStatus,
                noteState: NoteState_Old,
                noteId: newId,
                forkFrom: Some(forkFrom),
                editorContentStatus: Ec_Saved,
                noteLastEdited: Some(lastEdited),
                noteOwnerId: owner,
                hasSavePermission: true,
              },
              (_ => Router.push(Route.Note({noteId: newId, data: None}))),
            );
          | ForkStatus_Error =>
            Notify.error("Fork error");
            ReasonReact.Update({...state, forkStatus});
          | _ => ReasonReact.Update({...state, forkStatus})
          }
        },
      render: ({state, send}) => {
        let {editorContentStatus, lang} = state;
        Js.log(lang->langToString);
        <>
          <UI_Topbar.Actions>
            ...(
                 (~buttonClassName) =>
                   <>
                     <UI_Balloon
                       position=Down
                       length=Fit
                       message="Execute code (Ctrl+Enter)">
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
                     <Editor_Note_SaveButton
                       hasSavePermission=state.hasSavePermission
                       noteId=state.noteId
                       noteState=state.noteState
                       editorContentStatus
                       updateSaveStatus=(
                         saveStatus => saveStatus->UpdateNoteSaveStatus->send
                       )
                       getCurrentData=(
                         () => (
                           state.title,
                           Editor_Json.V1.encode(state.lang, state.blocks^),
                         )
                       )
                       registerShortcut
                       className=buttonClassName
                     />
                     <Editor_Note_ForkButton
                       hasSavePermission=state.hasSavePermission
                       noteId=state.noteId
                       noteState=state.noteState
                       updateForkStatus=(
                         forkStatus => forkStatus->UpdateForkStatus->send
                       )
                       getCurrentData=(
                         () => (
                           state.title,
                           Editor_Json.V1.encode(state.lang, state.blocks^),
                         )
                       )
                       className=buttonClassName
                       forkStatus=state.forkStatus
                     />
                     <UI_Balloon message="Sketch language" position=Down>
                       ...<fieldset
                            className="EditorNote__lang"
                            ariaLabel="Language toggle">
                            <span>
                              <input
                                type_="radio"
                                id="RE"
                                name="language"
                                checked=(lang == RE)
                                onChange=(_ => send(ChangeLang(RE)))
                              />
                              <label
                                htmlFor="RE" className="EditorNote__lang--RE">
                                "RE"->str
                              </label>
                            </span>
                            <span>
                              <input
                                type_="radio"
                                id="ML"
                                name="language"
                                checked=(lang == ML)
                                onChange=(_ => send(ChangeLang(ML)))
                              />
                              <label
                                htmlFor="ML" className="EditorNote__lang--ML">
                                "ML"->str
                              </label>
                            </span>
                          </fieldset>
                     </UI_Balloon>
                   </>
               )
          </UI_Topbar.Actions>
          <Helmet>
            <title>
              {
                let title = state.title == "" ? "untitled" : state.title;
                title->str;
              }
            </title>
          </Helmet>
          <main className="EditorNote Layout__center">
            <div className="EditorNote__metadata">
              <input
                className="EditorNote__metadata--title"
                placeholder="untitled sketch"
                value=state.title
                onChange=(event => valueFromEvent(event)->TitleUpdate->send)
              />
              <div className="EditorNote__metadata--info">
                <Editor_Note_GetUserInfo userId=state.noteOwnerId>
                  ...(
                       fun
                       | None => React.null
                       | Some(user) =>
                         <UI_SketchOwnerInfo
                           owner=user
                           noteLastEdited=?state.noteLastEdited
                           className="EditorNote__owner"
                         />
                     )
                </Editor_Note_GetUserInfo>
              </div>
              (
                state.forkFrom
                =>> (
                  forkFrom =>
                    <div className="EditorNote__forkFrom">
                      <p>
                        "Forked from"->str
                        <Router.Link
                          route=(Route.Note({noteId: forkFrom, data: None}))>
                          {j|/s/$(forkFrom)|j}->str
                        </Router.Link>
                      </p>
                    </div>
                )
              )
            </div>
            <Editor_Blocks
              lang
              blocks=state.blocks^
              registerExecuteCallback=(
                callback => send(RegisterExecuteCallback(callback))
              )
              registerShortcut
              onUpdate=(blocks => send(BlockUpdate(blocks)))
            />
          </main>
        </>;
      },
    };
  };
};

module WithShortcut = {
  let component = ReasonReact.statelessComponent("WithShortcut(Editor_Note)");

  let make =
      (
        ~hasSavePermission,
        ~noteId,
        ~noteState,
        ~lang=?,
        ~title=?,
        ~blocks,
        ~noteOwnerId,
        ~noteLastEdited,
        ~forkFrom=?,
        _children,
      ) => {
    ...component,
    render: _self =>
      <Shortcut.Consumer>
        ...(
             registerShortcut =>
               <Editor_Note
                 initialHasSavePermission=hasSavePermission
                 initialNoteId=noteId
                 initialNoteState=noteState
                 initialLang=?lang
                 initialTitle=?title
                 initialBlocks=blocks
                 initialNoteOwnerId=noteOwnerId
                 initialNoteLastEdited=noteLastEdited
                 initialForkFrom=?forkFrom
                 registerShortcut
               />
           )
      </Shortcut.Consumer>,
  };
};

include WithShortcut;
