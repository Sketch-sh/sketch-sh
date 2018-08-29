type state = {editor: ref(option(CodeMirror.editor))};

let component = ReasonReact.reducerComponent("Editor_CodeBlock");

let getEditor = (state, ~default, ~f) =>
  switch (state.editor^) {
  | None => default
  | Some(editor) => f(editor)
  };

[@bs.module "./cm_open_link.js"] external linkHandler: 'a => unit = "";
[@bs.module "./cm_open_link.js"] external register: 'a => unit = "";

let make =
    (
      ~value,
      ~focused,
      ~onChange,
      ~onFocus=?,
      ~onBlur=?,
      ~onBlockUp=?,
      ~onBlockDown=?,
      ~readOnly=?,
      _children,
    )
    : ReasonReact.component(state, _, unit) => {
  ...component,
  initialState: () => {editor: ref(None)},
  render: ({state}) =>
    <Editor_CodeMirror
      value
      focused
      onChange
      ?onFocus
      ?onBlur
      ?onBlockUp
      ?onBlockDown
      setEditor=(
        editor => {
          state.editor := Some(editor);
          register(editor);
          /* let wrapper = editor->CodeMirror.Editor.getWrapperElement;
             Webapi.Dom.(
               wrapper |> Element.addMouseDownEventListener(linkHandler)
             ); */
        }
      )
      options=(
        CodeMirror.EditorConfiguration.make(
          ~mode="gfm",
          ~theme=Config.cmTheme,
          ~placeholder="Add your thoughts here",
          ~lineNumbers=false,
          ~viewportMargin=infinity,
          ~lineWrapping=true,
          ~readOnly?,
          (),
        )
      )
    />,
};
