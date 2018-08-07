Modules.require("./Editor_Note.css");
open Utils;
open Editor_Types.Block;

type state = {
  title: string,
  pristine: bool,
  dirty: bool,
  blocks: ref(array(block)),
  isSaving: ref(bool),
};
type action =
  | TitleUpdate(string)
  | BlockUpdate(array(block));

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

let make = (~blocks, ~title="", ~loading as isSaving, ~onSave, _children) => {
  ...component,
  initialState: () => {
    title,
    pristine: true,
    dirty: false,
    blocks: ref(blocks),
    isSaving: ref(isSaving),
  },
  willReceiveProps: ({state}) =>
    if (state.isSaving^ != isSaving) {
      state.isSaving := isSaving;
      {...state, dirty: false, pristine: false};
    } else {
      state;
    },
  reducer: (action, state) =>
    switch (action) {
    | TitleUpdate(title) =>
      ReasonReact.Update({...state, pristine: false, dirty: true, title})
    | BlockUpdate(blocks) =>
      state.blocks := blocks;
      ReasonReact.Update({...state, pristine: false, dirty: true});
    },
  render: ({state, send}) => {
    let saveStatus =
      deriveSaveStatus(
        ~pristine=state.pristine,
        ~isSaving,
        ~dirty=state.dirty,
      );
    <>
      <UI_Topbar.WithToolbar>
        ...(
             (~buttonClassName) =>
               <>
                 <button className=buttonClassName>
                   <Fi.GitBranch />
                   "Fork"->str
                 </button>
                 <button className=buttonClassName>
                   <Fi.Terminal />
                   "Run"->str
                 </button>
                 <UI_Balloon
                   position=Down
                   message=(
                     switch (saveStatus) {
                     | Pristine => "Nothing to save"
                     | Saved => "Saved"
                     | Saving
                     | Unsaved => "Save modified changes"
                     }
                   )>
                   ...<button
                        disabled=(
                          switch (saveStatus) {
                          | Pristine
                          | Saved
                          | Saving => true
                          | Unsaved => false
                          }
                        )
                        className=buttonClassName
                        onClick=(
                          _ => onSave(~title=state.title, ~data=state.blocks^)
                        )>
                        (
                          isSaving ?
                            <Fi.Loader className="EditorNav__button--spin" /> :
                            <> <Fi.Save /> "Save"->str </>
                        )
                      </button>
                 </UI_Balloon>
               </>
           )
      </UI_Topbar.WithToolbar>
      <div className="EditorNote__saveStatus">
        {
          let status =
            switch (saveStatus) {
            | Pristine => ""
            | Saved => "Saved"
            | Saving => "Saving"
            | Unsaved => "Unsaved"
            };
          status->str;
        }
      </div>
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
          />
        </div>
        <Editor_Blocks
          blocks
          onUpdate=(blocks => send(BlockUpdate(blocks)))
        />
      </main>
    </>;
  },
};
