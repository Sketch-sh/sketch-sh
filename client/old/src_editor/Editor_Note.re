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
    isLinkMenuOpen: bool,
    links: array(Link.link),
    blocks: ref(array(Block.block)),
    editorContentStatus,
    executeCallback: option(unit => unit),
    noteOwnerId: id,
    noteLastEdited: option(Js.Json.t),
    forkStatus,
    isExecuting: bool,
  };

  type action =
    | TitleUpdate(string)
    | ToggleLinkMenu
    | LinkUpdate(array(Link.link))
    | BlockUpdate(array(Block.block))
    | RegisterExecuteCallback(unit => unit)
    | UpdateNoteSaveStatus(saveStatus)
    | UpdateForkStatus(forkStatus)
    | Execute
    | ChangeLang(lang)
    | IsExecuting(bool);

  let component = ReasonReact.reducerComponent("Editor_Page");

  [@react.component]
  let make =
      (
        ~initialHasSavePermission,
        ~initialNoteId,
        ~initialForkFrom=?,
        ~initialNoteState: noteState,
        ~initialLang: lang=RE,
        ~initialTitle: string="",
        ~initialLinks: array(Link.link),
        ~initialBlocks: array(Block.block),
        ~initialNoteOwnerId: id,
        ~initialNoteLastEdited: option(Js.Json.t),
        ~registerShortcut: Shortcut.subscribeFun,
        (),
      ) => {
    let makeInitialState = () => {
      hasSavePermission: initialHasSavePermission,
      noteId: initialNoteId,
      forkFrom: initialForkFrom,
      noteState: initialNoteState,
      lang: initialLang,
      title: initialTitle,
      editorContentStatus: Ec_Pristine,
      isLinkMenuOpen: false,
      links: initialLinks,
      blocks: ref(initialBlocks),
      executeCallback: None,
      noteOwnerId: initialNoteOwnerId,
      noteLastEdited: initialNoteLastEdited,
      forkStatus: ForkStatus_Initial,
      isExecuting: false,
    };
    ReactCompat.useRecordApi({
      ...component,
      initialState: makeInitialState,
      didUpdate: ({oldSelf, newSelf}) =>
        if (newSelf.state.editorContentStatus
            != oldSelf.state.editorContentStatus) {
          /* This execute the code after save */
          if (newSelf.state.editorContentStatus == Ec_Saved) {
            newSelf.send(Execute);
          };
        },
      didMount: self => {
        let unloadHandler = (message, self) => {
          message :=
            (
              switch (self.ReasonReact.state.editorContentStatus) {
              | Ec_Saved
              | Ec_Pristine => None
              | _ => Some("Changes you made may not be saved")
              }
            );
          ();
        };
        Router.Unload.register(self.handle(unloadHandler));
        self.onUnmount(Router.Unload.unregister);
      },
      reducer: (action, state) =>
        switch (action) {
        | TitleUpdate(title) =>
          ReasonReact.Update({...state, title, editorContentStatus: Ec_Dirty})
        | RegisterExecuteCallback(callback) =>
          ReasonReact.Update({...state, executeCallback: Some(callback)})
        | Execute =>
          ReasonReact.SideEffects(
            ({state}) =>
              switch (state.executeCallback) {
              | None => ()
              | Some(callback) => callback()
              },
          )
        | ToggleLinkMenu =>
          ReasonReact.Update({
            ...state,
            isLinkMenuOpen: !state.isLinkMenuOpen,
          })

        | LinkUpdate(links) =>
          ReasonReact.UpdateWithSideEffects(
            {...state, links, editorContentStatus: Ec_Dirty},
            _ =>
              switch (state.executeCallback) {
              | None => ()
              | Some(callback) => callback()
              },
          )
        | BlockUpdate(blocks) =>
          state.blocks := blocks;
          ReasonReact.Update({...state, editorContentStatus: Ec_Dirty});
        | ChangeLang(lang) =>
          let refmtNotificationKey = "rtop:refmt-acknowledge";
          Dom.Storage.(
            switch (localStorage |> getItem(refmtNotificationKey)) {
            | Some(_) => ()
            | None =>
              Notify.info(
                "When you change languages, we will auto-format your code into the target language's syntax. Multiple transformations result in unusual formatting with large code blocks, and comments are removed when converting ReasonML into OCaml. To avoid this, we preserve the previous state, but once you make an edit after changing languages, there will be side-effects.",
                ~sticky=true,
              );
              localStorage |> setItem(refmtNotificationKey, "acknowledged");
            }
          );
          ReasonReact.Update({...state, lang, editorContentStatus: Ec_Dirty});
        | UpdateNoteSaveStatus(saveStatus) =>
          switch (state.editorContentStatus, saveStatus) {
          | (_, SaveStatus_Initial) => ReasonReact.NoUpdate
          | (_, SaveStatus_Loading) =>
            ReasonReact.Update({...state, editorContentStatus: Ec_Saving})
          | (Ec_Dirty, SaveStatus_Done({lastEdited: _})) =>
            ReasonReact.Update({...state, editorContentStatus: Ec_Dirty})
          | (_, SaveStatus_Done({lastEdited})) =>
            Notify.success("Saved successfully", ~sticky=false);
            ReasonReact.UpdateWithSideEffects(
              {
                ...state,
                editorContentStatus: Ec_Saved,
                noteState: NoteState_Old,
                noteLastEdited: Some(lastEdited),
              },
              _ =>
                Router.pushSilent(
                  Route.Note({noteId: state.noteId, data: None}),
                ),
            );
          | (_, SaveStatus_Error) =>
            Notify.error("Save error");
            ReasonReact.Update({...state, editorContentStatus: Ec_Dirty});
          }
        | UpdateForkStatus(forkStatus) =>
          switch (forkStatus) {
          | ForkStatus_Done({newId, forkFrom, lastEdited, owner}) =>
            Notify.success("Forked successfully", ~sticky=false);
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
              _ => Router.push(Route.Note({noteId: newId, data: None})),
            );
          | ForkStatus_Error =>
            Notify.error("Fork error");
            ReasonReact.Update({...state, forkStatus});
          | _ => ReasonReact.Update({...state, forkStatus})
          }
        | IsExecuting(isExecuting) =>
          ReasonReact.Update({...state, isExecuting})
        },
      render: ({state, send}) => {
        let {editorContentStatus, lang} = state;
        <>
          <UI_Topbar.Actions>
            ...{(~buttonClassName) =>
              <>
                <UI_Balloon
                  position=Down length=Fit message="Execute code (Ctrl+Enter)">
                  ...<button
                       disabled={state.isExecuting}
                       className=buttonClassName
                       onClick={_ =>
                         switch (state.executeCallback) {
                         | None => ()
                         | Some(callback) => callback()
                         }
                       }>
                       <UI_LoadingWrapper
                         loading={state.isExecuting} delayMs=500>
                         ...{loading =>
                           loading
                             ? <Fi.Loader
                                 className="EditorNav__button--spin"
                               />
                             : <Fi.Play />
                         }
                       </UI_LoadingWrapper>
                       <span> "Run"->str </span>
                     </button>
                </UI_Balloon>
                <Editor_Note_SaveButton
                  hasSavePermission={state.hasSavePermission}
                  noteId={state.noteId}
                  noteState={state.noteState}
                  editorContentStatus
                  updateSaveStatus={saveStatus =>
                    saveStatus->UpdateNoteSaveStatus->send
                  }
                  getCurrentData={() =>
                    (
                      state.title,
                      Editor_Json.V1.encode(
                        state.lang,
                        state.links,
                        Editor_Blocks_Utils.filterDeletedBlocks(
                          state.blocks^,
                        ),
                      ),
                    )
                  }
                  registerShortcut
                  className=buttonClassName
                />
                <Editor_Note_ForkButton
                  hasSavePermission={state.hasSavePermission}
                  noteId={state.noteId}
                  noteState={state.noteState}
                  updateForkStatus={forkStatus =>
                    forkStatus->UpdateForkStatus->send
                  }
                  getCurrentData={() =>
                    (
                      state.title,
                      Editor_Json.V1.encode(
                        state.lang,
                        state.links,
                        Editor_Blocks_Utils.filterDeletedBlocks(
                          state.blocks^,
                        ),
                      ),
                    )
                  }
                  className=buttonClassName
                  forkStatus={state.forkStatus}
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
                           checked={lang == RE}
                           onChange={_ => send(ChangeLang(RE))}
                         />
                         <label htmlFor="RE" className="EditorNote__lang--RE">
                           "RE"->str
                         </label>
                       </span>
                       <span>
                         <input
                           type_="radio"
                           id="ML"
                           name="language"
                           checked={lang == ML}
                           onChange={_ => send(ChangeLang(ML))}
                         />
                         <label htmlFor="ML" className="EditorNote__lang--ML">
                           "ML"->str
                         </label>
                       </span>
                     </fieldset>
                </UI_Balloon>
              </>
            }
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
                value={state.title}
                onChange={event => valueFromEvent(event)->TitleUpdate->send}
              />
              <div className="EditorNote__metadata--info">
                <Editor_Note_GetUserInfo userId={state.noteOwnerId}>
                  ...{
                       fun
                       | None => ReasonReact.null
                       | Some((user: Js.t('a))) =>
                         <UI_SketchOwnerInfo
                           owner=user
                           noteLastEdited=?{state.noteLastEdited}
                           className="EditorNote__owner"
                         />
                     }
                </Editor_Note_GetUserInfo>
                <UI_Balloon position=Down message="Link sketches">
                  ...<button
                       className={ClassNames.make([
                         "EditorNote__linkMenu",
                         state.isLinkMenuOpen
                         ->ClassNames.ifTrue("EditorNote__linkMenu--open"),
                       ])}
                       onClick={_ => send(ToggleLinkMenu)}>
                       <Fi.Link />
                     </button>
                </UI_Balloon>
                <i className="EditorNote__links">
                  {state.links
                   ->(
                       Belt.Array.mapU((. link) =>
                         link
                         ->Editor_Links_Utils.getNameFromLink
                         ->String.capitalize
                       )
                     )
                   ->joinWithComma
                   ->str}
                </i>
              </div>
              {state.forkFrom
               =>> (
                 forkFrom =>
                   <div className="EditorNote__forkFrom">
                     <p>
                       "Forked from"->str
                       <Router.Link
                         route={Route.Note({noteId: forkFrom, data: None})}>
                         {j|/s/$(forkFrom)|j}->str
                       </Router.Link>
                     </p>
                   </div>
               )}
            </div>
            {state.isLinkMenuOpen
               ? <Editor_Links
                   key={
                     state.noteId ++ string_of_int(Array.length(state.links))
                   }
                   currentSketchId={state.noteId}
                   links={state.links}
                   onUpdate={links => send(LinkUpdate(links))}
                 />
               : ReasonReact.null}
            <Editor_Blocks
              key={state.noteId}
              lang
              blocks=state.blocks^
              links={state.links}
              registerExecuteCallback={callback =>
                send(RegisterExecuteCallback(callback))
              }
              registerShortcut
              onUpdate={blocks => send(BlockUpdate(blocks))}
              onExecute={isExecuting => send(IsExecuting(isExecuting))}
            />
          </main>
        </>;
      },
    });
  };
};

module WithShortcut = {
  let component = ReasonReact.statelessComponent("WithShortcut(Editor_Note)");

  [@react.component]
  let make =
      (
        ~hasSavePermission,
        ~noteId,
        ~noteState,
        ~lang=?,
        ~title=?,
        ~links,
        ~blocks,
        ~noteOwnerId,
        ~noteLastEdited,
        ~forkFrom=?,
        (),
      ) =>
    ReactCompat.useRecordApi({
      ...component,
      render: _self =>
        <Shortcut.Consumer>
          ...{registerShortcut =>
            <Editor_Note
              initialHasSavePermission=hasSavePermission
              initialNoteId=noteId
              initialNoteState=noteState
              initialLang=?lang
              initialTitle=?title
              initialLinks=links
              initialBlocks=blocks
              initialNoteOwnerId=noteOwnerId
              initialNoteLastEdited=noteLastEdited
              initialForkFrom=?forkFrom
              registerShortcut
            />
          }
        </Shortcut.Consumer>,
    });
};

include WithShortcut;
