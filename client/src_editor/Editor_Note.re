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

let make = (~blocks, ~title="", ~loading, ~onSave, _children) => {
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
    <div className="pageSizer">
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
      <button onClick=(_ => onSave(~title=state.title, ~data=blocks))>
        (loading ? "Saving..." |. str : "Save this notebook" |. str)
      </button>
      <Editor_Blocks blocks onUpdate=(blocks => send(BlockUpdate(blocks))) />
    </div>,
};
