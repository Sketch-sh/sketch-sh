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
    <div>
      <aside className="EditorNav">
        <div className="EditorNav__top">
          <Fi.IconContext.Provider
            value={"className": "EditorNav__button--icon"}>
            <div className="EditorNav__button">
              <UI_Balloon position=Right message="Home">
                ...<button
                     className="EditorNav__button--button"
                     onClick=(_ => Router.push(Route.Home))>
                     <Fi.Home />
                   </button>
              </UI_Balloon>
            </div>
            <div className="EditorNav__button">
              <UI_Balloon
                position=Right
                message=(
                  switch (saveStatus) {
                  | Pristine => "Nothing to save"
                  | Saved => "Already saved !"
                  | Saving
                  | Unsaved => "Save"
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
                     className="EditorNav__button--button"
                     onClick=(
                       _ => onSave(~title=state.title, ~data=state.blocks^)
                     )>
                     (
                       isSaving ?
                         <Fi.Loader className="EditorNav__button--spin" /> :
                         <Fi.Save />
                     )
                   </button>
              </UI_Balloon>
            </div>
          </Fi.IconContext.Provider>
        </div>
        <div className="EditorNav__bottom">
          <span className="EditorNav__button--saveIndicator ">
            (
              (
                switch (saveStatus) {
                | Pristine => ""
                | Saved => "Saved"
                | Saving => "Saving"
                | Unsaved => "Unsaved"
                }
              )
              |. str
            )
          </span>
        </div>
      </aside>
      <main className="pageSizer">
        <Helmet>
          <title>
            ((state.title == "" ? "untitled" : state.title) |. str)
          </title>
        </Helmet>
        <div className="metadata">
          <input
            className="metadata__title-input"
            placeholder="untitled note"
            value=state.title
            onChange=(event => valueFromEvent(event) |. TitleUpdate |. send)
          />
        </div>
        <Editor_Blocks
          blocks
          onUpdate=(blocks => send(BlockUpdate(blocks)))
        />
      </main>
    </div>;
  },
};
