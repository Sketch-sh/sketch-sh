let component = ReasonReact.statelessComponent("Editor_CodeBlock");

let make =
    (~value, ~firstLineNumber, ~widgets, ~onChange, ~onExecute, _children) => {
  ...component,
  render: _self =>
    <Editor_CodeMirror
      value
      firstLineNumber
      onChange
      widgets
      options=(
        CodeMirror.EditorConfiguration.make(
          ~mode="rust",
          ~theme="runkit-light",
          ~lineNumbers=true,
          ~viewportMargin=infinity,
          ~matchBrackets=true,
          ~tabSize=2,
          ~firstLineNumber,
          ~extraKeys={
            let key = Js.Dict.empty();
            key
            |. Js.Dict.set(
                 "Tab",
                 cm => {
                   let spaces =
                     String.make(
                       cm |. CodeMirror.Editor.GetOption.indentUnit,
                       ' ',
                     );
                   cm |. CodeMirror.Editor.replaceSelection(spaces);
                 },
               );
            key |. Js.Dict.set("Shift-Enter", _cm => onExecute());
            key;
          },
          (),
        )
      )
    />,
};
