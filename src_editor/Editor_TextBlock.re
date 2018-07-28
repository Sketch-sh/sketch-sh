type state = {editor: ref(option(CodeMirror.editor))};

let component = ReasonReact.reducerComponent("Editor_CodeBlock");

let getEditor = (state, ~default, ~f) =>
  switch (state.editor^) {
  | None => default
  | Some(editor) => f(editor)
  };

let make =
    (~value, ~onChange, _children)
    : ReasonReact.component(state, _, unit) => {
  ...component,
  initialState: () => {editor: ref(None)},
  render: ({state}) =>
    <Editor_CodeMirror
      value
      onChange
      setEditor=(
        editor => {
          state.editor := Some(editor);
          ();
        }
      )
      options=(
        CodeMirror.EditorConfiguration.make(
          ~mode="gfm",
          ~theme="runkit-light",
          ~lineNumbers=false,
          ~viewportMargin=infinity,
          ~lineWrapping=true,
          (),
        )
      )
    />,
};
