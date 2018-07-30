open Utils;
type state = {title: string};
type action =
  | TitleUpdate(string);

let component = ReasonReact.reducerComponent("Editor_Page");

let make = (~blocks, _children) => {
  ...component,
  initialState: () => {title: ""},
  reducer: (action, _state) =>
    switch (action) {
    | TitleUpdate(title) => ReasonReact.Update({title: title})
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
          className="metadata-input"
          placeholder="untitled note"
          value=state.title
          onChange=(event => valueFromEvent(event) |. TitleUpdate |. send)
        />
      </div>
      <Editor_Blocks blocks />
    </div>,
};
