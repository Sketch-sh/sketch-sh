open Utils;
type state = {title: string};
type action =
  | TitleUpdate(string);

let component = ReasonReact.reducerComponent("Editor_Page");

let make = (~blocks, ~title: option(string)=?, _children) => {
  ...component,
  initialState: () => {
    title:
      switch (title) {
      | None => ""
      | Some(title) => title
      },
  },
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
        <div className="metadata__title">
          <span className="metadata__title-sharp"> ("#" |. str) </span>
          <input
            className="metadata__title-input"
            placeholder="untitled note"
            value=state.title
            onChange=(event => valueFromEvent(event) |. TitleUpdate |. send)
          />
        </div>
      </div>
      <Editor_Blocks blocks />
    </div>,
};
