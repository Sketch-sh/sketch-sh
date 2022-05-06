[@react.component]
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
    ) => {
  let state = React.useState({editor: ref(none)});

  <Editor_CodeMirror
    value
    focused
    onChange
    ?onFocus
    ?onBlur
    ?onBlockUp
    ?onBlockDown
    setEditor={editor => {
      state.editor := Some(editor);
      CodeMirror_Links.register(editor);
    }}
    options={CodeMirror.EditorConfiguration.make(
      ~mode="gfm",
      ~theme=Config.cmTheme,
      ~placeholder="Add your thoughts here",
      ~lineNumbers=false,
      ~viewportMargin=infinity,
      ~lineWrapping=true,
      ~readOnly?,
      (),
    )}
  />;
};
