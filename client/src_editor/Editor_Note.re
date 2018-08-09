Modules.require("./Editor_Note.css");
open Utils;
open Editor_Types.Block;

type state = {
  title: string,
  pristine: bool,
  dirty: bool,
  blocks: ref(array(block)),
  isSaving: ref(bool),
  executeCallback: option(unit => unit),
};
type action =
  | TitleUpdate(string)
  | BlockUpdate(array(block))
  | RegisterExecuteCallback(unit => unit);

type saveStatus =
  | Pristine
  | Saved
  | Saving
  | Unsaved;

let deriveSaveStatus = (~pristine, ~isSaving, ~dirty) =>
  if (pristine) {
    Pristine;
  } else if (isSaving) {
    Saving;
  } else if (dirty) {
    Unsaved;
  } else {
    Saved;
  };

let component = ReasonReact.reducerComponent("Editor_Page");

let make =
    (
      ~blocks,
      ~noteOwnerId=?,
      ~title="",
      ~noteSaveStatus: NoteSave_Types.noteSaveStatus,
      ~isEditable,
      ~onSave,
      _children,
    ) => {
  ...component,
  initialState: () => {
    title,
    pristine: true,
    dirty: false,
    blocks: ref(blocks),
    isSaving: ref(false),
    executeCallback: None,
  },
  /* willReceiveProps: ({state}) =>
     if (state.isSaving^ != isSaving) {
       state.isSaving := isSaving;
       {...state, dirty: false, pristine: false};
     } else {
       state;
     }, */
  reducer: (action, state) =>
    switch (action) {
    | TitleUpdate(title) =>
      ReasonReact.Update({...state, pristine: false, dirty: true, title})
    | RegisterExecuteCallback(callback) =>
      ReasonReact.Update({...state, executeCallback: Some(callback)})
    | BlockUpdate(blocks) =>
      state.blocks := blocks;
      ReasonReact.Update({...state, pristine: false, dirty: true});
    },
  render: ({state, send}) => {
    Js.log(
      switch (noteSaveStatus) {
      | NoteSave_Done => "note save done"
      | NoteSave_Loading => "note save loading"
      | NoteSave_Error => "note save error"
      },
    );
    <>
      /* let saveStatus =
         deriveSaveStatus(
           ~pristine=state.pristine,
           ~isSaving,
           ~dirty=state.dirty,
         ); */
      <UI_Topbar.WithToolbar>
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
                   !isEditable ?
                     /* aka readOnly */
                     ReasonReact.null :
                     <UI_Balloon position=Down length=Fit message="Save">
                       /* switch (saveStatus) {
                          | Pristine => "Nothing to save (Ctrl+S)"
                          | Saved => "Saved (Ctrl+S)"
                          | Saving
                          | Unsaved => "Save modified changes (Ctrl+S)"
                          } */
                       /* switch (saveStatus) {
                          | Pristine
                          | Saved
                          | Saving => true
                          | Unsaved => false
                          } */

                         ...<button
                              disabled=false
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
                              <> <Fi.Save /> "Save"->str </>
                            </button>
                       </UI_Balloon>
                 )
                 /* (
                      isSaving ?
                        <>
                          <Fi.Loader
                            className="EditorNav__button--spin"
                          />
                          "Saving"->str
                        </> :
                        <> <Fi.Save /> "Save"->str </>
                    ) */
               </>
           )
      </UI_Topbar.WithToolbar>
      <div
        className="EditorNote__saveStatus"
        /* {
             let status =
               switch (saveStatus) {
               | Pristine => ""
               | Saved => "Saved"
               | Saving => "Saving"
               | Unsaved => "Unsaved"
               };
             status->str;
           } */
      />
      <main className="EditorNote">
        <Helmet>
          <title>
            {
              let title = state.title == "" ? "untitled" : state.title;
              title->str;
            }
          </title>
        </Helmet>
        <div className="metadata">
          <input
            className="metadata__title-input"
            placeholder="untitled note"
            value=state.title
            onChange=(event => valueFromEvent(event)->TitleUpdate->send)
            readOnly=(!isEditable)
          />
        </div>
        <Editor_Blocks
          blocks
          registerExecuteCallback=(
            callback => send(RegisterExecuteCallback(callback))
          )
          onUpdate=(blocks => send(BlockUpdate(blocks)))
          readOnly=(!isEditable)
        />
      </main>
    </>;
  },
};
