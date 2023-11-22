Modules.require("./Editor_Note.css");
open Utils;
open Editor_Types;

module Editor_Note = {
  type state = {
    hasSavePermission: bool,
    noteId: id,
    compilerVersion: CompilerVersion.t,
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
    | ChangeLang(lang)
    | IsExecuting(bool);

  let reducer = (state, action) => {
    switch (action) {
    | TitleUpdate(title) => {...state, title, editorContentStatus: Ec_Dirty}
    | RegisterExecuteCallback(callback) => {
        ...state,
        executeCallback: Some(callback),
      }
    | ToggleLinkMenu => {...state, isLinkMenuOpen: !state.isLinkMenuOpen}
    | LinkUpdate(links) => {...state, links, editorContentStatus: Ec_Dirty}
    | BlockUpdate(blocks) =>
      state.blocks := blocks;
      {...state, editorContentStatus: Ec_Dirty};
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
      {...state, lang, editorContentStatus: Ec_Dirty};
    | UpdateNoteSaveStatus(saveStatus) =>
      switch (state.editorContentStatus, saveStatus) {
      | (_, SaveStatus_Initial) => state
      | (_, SaveStatus_Loading) => {...state, editorContentStatus: Ec_Saving}
      | (Ec_Dirty, SaveStatus_Done({lastEdited: _})) => {
          ...state,
          editorContentStatus: Ec_Dirty,
        }
      | (_, SaveStatus_Done({lastEdited})) =>
        Notify.success("Saved successfully", ~sticky=false);
        {
          ...state,
          editorContentStatus: Ec_Saved,
          noteState: NoteState_Old,
          noteLastEdited: Some(lastEdited),
        };
      | (_, SaveStatus_Error) =>
        Notify.error("Save error");
        {...state, editorContentStatus: Ec_Dirty};
      }
    | UpdateForkStatus(forkStatus) =>
      switch (forkStatus) {
      | ForkStatus_Done({newId, forkFrom, lastEdited, owner}) =>
        Notify.success("Forked successfully", ~sticky=false);
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
        };
      | ForkStatus_Error =>
        Notify.error("Fork error");
        {...state, forkStatus};
      | _ => {...state, forkStatus}
      }
    | IsExecuting(isExecuting) => {...state, isExecuting}
    };
  };

  [@react.component]
  let make =
      (
        ~initialHasSavePermission,
        ~initialNoteId,
        ~initialCompilerVersion,
        ~initialForkFrom=?,
        ~initialNoteState: noteState,
        ~initialLang: lang=ML,
        ~initialTitle: string="",
        ~initialLinks: array(Link.link),
        ~initialBlocks: array(Block.block),
        ~initialNoteOwnerId: id,
        ~initialNoteLastEdited: option(Js.Json.t),
        ~registerShortcut: Shortcut.subscribeFun,
        ~refetch=?,
      ) => {
    let ({editorContentStatus, lang} as state, dispatch) =
      React.useReducer(
        reducer,
        {
          hasSavePermission: initialHasSavePermission,
          noteId: initialNoteId,
          compilerVersion: initialCompilerVersion,
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
        },
      );

    let execute = () => {
      switch (state.executeCallback) {
      | None => ()
      | Some(callback) => callback()
      };
    };

    React.useEffect0(() => {
      let unloadHandler = message => {
        message :=
          (
            switch (state.editorContentStatus) {
            | Ec_Saved
            | Ec_Pristine => None
            | _ => Some("Changes you made may not be saved")
            }
          );
        ();
      };
      Router.Unload.register(unloadHandler);
      Some(Router.Unload.unregister);
    });

    React.useEffect1(
      () => {
        if (editorContentStatus == Ec_Saved) {
          execute();
        };
        None;
      },
      [|editorContentStatus|],
    );

    React.useEffect1(
      () => {
        execute();
        None;
      },
      [|state.links|],
    );

    React.useEffect1(
      () => {
        Router.pushSilent(Route.Note({noteId: state.noteId, data: None}));
        None;
      },
      [|state.noteId|],
    );

    React.useEffect1(
      () => {
        switch (refetch) {
        | None => None
        | Some(refetch) =>
          let intervalId =
            Js.Global.setInterval(
              () => {
                refetch(None)
                ->Js.Promise.then_(
                    value => {
                      Js.log(value);
                      Js.Promise.resolve();
                    },
                    _,
                  )
                ->ignore
              },
              5000,
            );
          Some(() => Js.Global.clearInterval(intervalId));
        }
      },
      [|refetch|],
    );

    <>
      <UI_Topbar.Actions>
        {(~buttonClassName) =>
           <>
             <UI_Balloon
               position=UI_Balloon.Down
               length=UI_Balloon.Fit
               message="Execute code (Ctrl+Enter)">
               <button
                 disabled={state.isExecuting}
                 className=buttonClassName
                 onClick={_ =>
                   switch (state.executeCallback) {
                   | None => ()
                   | Some(callback) => callback()
                   }
                 }>
                 <UI_LoadingWrapper loading={state.isExecuting} delayMs=500>
                   {loading =>
                      loading
                        ? <Fi.Loader className="EditorNav__button--spin" />
                        : <Fi.Play />}
                 </UI_LoadingWrapper>
                 <span> "Run"->str </span>
               </button>
             </UI_Balloon>
             <Editor_Note_SaveButton
               hasSavePermission={state.hasSavePermission}
               noteId={state.noteId}
               compilerVersion={state.compilerVersion}
               noteState={state.noteState}
               editorContentStatus
               updateSaveStatus={saveStatus =>
                 dispatch @@ UpdateNoteSaveStatus(saveStatus)
               }
               getCurrentData={() =>
                 (
                   state.title,
                   Editor_Json.V1.encode(
                     state.lang,
                     state.links,
                     Editor_Blocks_Utils.filterDeletedBlocks(state.blocks^),
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
               compilerVersion={state.compilerVersion}
               updateForkStatus={forkStatus =>
                 dispatch @@ UpdateForkStatus(forkStatus)
               }
               getCurrentData={() =>
                 (
                   state.title,
                   Editor_Json.V1.encode(
                     state.lang,
                     state.links,
                     Editor_Blocks_Utils.filterDeletedBlocks(state.blocks^),
                   ),
                 )
               }
               className=buttonClassName
               forkStatus={state.forkStatus}
             />
             <UI_Balloon message="Sketch language" position=UI_Balloon.Down>
               <fieldset
                 className="EditorNote__lang" ariaLabel="Language toggle">
                 <span>
                   <input
                     type_="radio"
                     id="ML"
                     name="language"
                     checked={lang == ML}
                     onChange={_ => dispatch @@ ChangeLang(ML)}
                   />
                   <label htmlFor="ML" className="EditorNote__lang--ML">
                     "ML"->str
                   </label>
                 </span>
                 <span>
                   <input
                     type_="radio"
                     id="RE"
                     name="language"
                     checked={lang == RE}
                     onChange={_ => dispatch @@ ChangeLang(RE)}
                   />
                   <label htmlFor="RE" className="EditorNote__lang--RE">
                     "RE"->str
                   </label>
                 </span>
               </fieldset>
             </UI_Balloon>
             <div>
               {React.string("foo")}
               <input
                 name="isGoing"
                 type_="checkbox"
                 checked=true
                 onChange={event => {
                   Webapi.Dom.(event->ReactEvent.Form.preventDefault)
                 }}
               />
             </div>
           </>}
      </UI_Topbar.Actions>
      <Helmet>
        <title>
          {let title = state.title == "" ? "untitled" : state.title;
           title->str}
        </title>
      </Helmet>
      <main className="EditorNote Layout__center">
        <div className="EditorNote__metadata">
          <input
            className="EditorNote__metadata--title"
            placeholder="untitled sketch"
            value={state.title}
            onChange={event =>
              dispatch @@ TitleUpdate(valueFromEvent(event))
            }
          />
          <div className="EditorNote__metadata--info">
            <Editor_Note_GetUserInfo userId={state.noteOwnerId}>
              {fun
               | None => React.null
               | Some(user: Js.t('a)) =>
                 <UI_SketchOwnerInfo
                   owner=user
                   noteLastEdited=?{state.noteLastEdited}
                   noteCompilerVersion={state.compilerVersion}
                   className="EditorNote__owner"
                 />}
            </Editor_Note_GetUserInfo>
            <UI_Balloon position=UI_Balloon.Down message="Link sketches">
              <button
                className={ClassNames.make([
                  "EditorNote__linkMenu",
                  state.isLinkMenuOpen
                  ->ClassNames.ifTrue("EditorNote__linkMenu--open"),
                ])}
                onClick={_ => dispatch @@ ToggleLinkMenu}>
                <Fi.Link />
              </button>
            </UI_Balloon>
            <i className="EditorNote__links">
              {state.links
               ->(
                   Belt.Array.mapU((. link) =>
                     link
                     ->Editor_Links_Utils.getNameFromLink
                     ->String.capitalize_ascii
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
               key={state.noteId ++ string_of_int(Array.length(state.links))}
               currentSketchId={state.noteId}
               links={state.links}
               onUpdate={links => dispatch @@ LinkUpdate(links)}
             />
           : React.null}
        <Editor_Blocks
          key={state.noteId}
          lang
          compilerVersion={state.compilerVersion}
          blocks=state.blocks^
          links={state.links}
          registerExecuteCallback={callback =>
            dispatch @@ RegisterExecuteCallback(callback)
          }
          registerShortcut
          onUpdate={blocks => dispatch @@ BlockUpdate(blocks)}
          onExecute={isExecuting => dispatch @@ IsExecuting(isExecuting)}
        />
      </main>
    </>;
  };
};

module WithShortcut = {
  [@react.component]
  let make =
      (
        ~hasSavePermission,
        ~noteId,
        ~noteState,
        ~compilerVersion,
        ~lang=?,
        ~title=?,
        ~links,
        ~blocks,
        ~noteOwnerId,
        ~noteLastEdited,
        ~forkFrom=?,
        ~refetch=?,
      ) =>
    ReactCompat.useRecordApi({
      ...ReactCompat.component,
      render: _self =>
        <Shortcut.Consumer>
          {registerShortcut =>
             <Editor_Note
               initialHasSavePermission=hasSavePermission
               initialNoteId=noteId
               initialNoteState=noteState
               initialCompilerVersion=compilerVersion
               initialLang=?lang
               initialTitle=?title
               initialLinks=links
               initialBlocks=blocks
               initialNoteOwnerId=noteOwnerId
               initialNoteLastEdited=noteLastEdited
               initialForkFrom=?forkFrom
               registerShortcut
               ?refetch
             />}
        </Shortcut.Consumer>,
    });
};

include WithShortcut;
