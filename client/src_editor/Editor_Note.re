Modules.require("./Editor_Note.css");
open Utils;
open Editor_Types.Block;

type state = {
  title: string,
  blocks: ref(array(block)),
};
type action =
  | TitleUpdate(string)
  | BlockUpdate(array(block));

let component = ReasonReact.reducerComponent("Editor_Page");

let make = (~blocks, ~title="", ~loading as isSaving, ~onSave, _children) => {
  ...component,
  initialState: () => {title, blocks: ref(blocks)},
  reducer: (action, state) =>
    switch (action) {
    | TitleUpdate(title) => ReasonReact.Update({...state, title})
    | BlockUpdate(blocks) =>
      state.blocks := blocks;
      ReasonReact.NoUpdate;
    },
  render: ({state, send}) =>
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
            ("Saved" |. str)
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
    </div>,
};
