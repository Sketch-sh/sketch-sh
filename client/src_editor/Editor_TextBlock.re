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
  <Editor_CodeMirror
    value
    focused
    onChange
    ?onFocus
    ?onBlur
    ?onBlockUp
    ?onBlockDown
    setEditor={editor => {CodeMirror_Links.register(editor)}}
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
