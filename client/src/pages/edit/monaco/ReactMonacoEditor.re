open MonacoEditor;

[@react.component] [@bs.module "react-monaco-editor"]
external make:
  (
    ~width: int=?,
    ~height: int=?,
    ~language: string=?,
    ~theme: string=?,
    ~value: string=?,
    ~defaultValue: string=?,
    ~editorWillMount: monaco => unit=?,
    ~editorDidMount: (IStandaloneCodeEditor.t, monaco) => unit=?,
    ~ref: React.Ref.t(Js.Nullable.t({. "editor": IStandaloneCodeEditor.t}))=?,
    ~onChange: (string, IModelContentChangedEvent.t) => unit
  ) =>
  React.element =
  "default";
