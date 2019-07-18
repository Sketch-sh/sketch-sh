open CodeMirror;

module Controlled = {
  [@bs.module "react-codemirror2"] [@react.component]
  external make:
    (
      ~value: string,
      ~options: EditorConfiguration.t,
      ~onChange: (editor, EditorChange.t, string) => unit=?,
      ~onBeforeChange: (editor, EditorChange.t, string) => unit=?,
      ~editorDidMount: editor => unit=?,
      ~className: string=?
    ) =>
    React.element =
    "Controlled";
};
