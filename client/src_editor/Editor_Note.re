Modules.require("./Editor_Note.css");
open Utils;
open Editor_Types.Block;

type state = {
  title: string,
  dirty: bool,
  blocks: ref(array(block)),
  isSaving: ref(bool),
};
type action =
  | TitleUpdate(string)
  | BlockUpdate(array(block));

type saveStatus =
  | Saved
  | Saving
  | Unsaved;

let deriveSaveStatus = (isSaving, dirty) =>
  if (isSaving) {
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
    dirty: false,
    blocks: ref(blocks),
    isSaving: ref(isSaving),
  },
  willReceiveProps: ({state}) =>
    if (state.isSaving^ != isSaving) {
      state.isSaving := isSaving;
      {...state, dirty: false};
    } else {
      state;
    },
  reducer: (action, state) =>
    switch (action) {
    | TitleUpdate(title) =>
      ReasonReact.Update({...state, dirty: true, title})
    | BlockUpdate(blocks) =>
      state.blocks := blocks;
      ReasonReact.Update({...state, dirty: true});
    },
  render: ({state, send}) => {
    let saveStatus = deriveSaveStatus(isSaving, state.dirty);
    <div>
      <aside className="EditorNav">
        <div className="EditorNav__top">
          <Fi.IconContext.Provider
            value={"className": "EditorNav__button--icon"}>
            <div className="EditorNav__button">
              (
                ReasonReact.cloneElement(
                  <button
                    className="EditorNav__button--button"
                    onClick=(_ => Router.push(Route.Home))>
                    <Fi.Home />
                  </button>,
                  ~props={
                    "data-balloon": "Home",
                    "data-balloon-pos": "right",
                  },
                  [||],
                )
              )
            </div>
            <div className="EditorNav__button">
              (
                ReasonReact.cloneElement(
                  <button
                    disabled=(
                      switch (saveStatus) {
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
                  </button>,
                  ~props={
                    "data-balloon": "Save",
                    "data-balloon-pos": "right",
                  },
                  [||],
                )
              )
            </div>
          </Fi.IconContext.Provider>
        </div>
        <div className="EditorNav__bottom">
          <span className="EditorNav__button--saveIndicator ">
            (
              (
                switch (saveStatus) {
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
