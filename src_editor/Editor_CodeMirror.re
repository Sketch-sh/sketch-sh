%bs.raw
{|
/* import "codemirror/mode/mllike/mllike" */
import "./reason-mode.js"
import "codemirror/addon/edit/matchbrackets"
import "codemirror/lib/codemirror.css"

import "./cm-theme.css"
import "./source-editor.css"
import "./app.css" /* <-- Important, no semi */
|};

/* open Utils; */

type state = {
  editor: ref(option(CodeMirror.editor)),
  divRef: ref(option(Dom.element)),
  value: string,
};

let setDivRef = (theRef, {ReasonReact.state}) =>
  state.divRef := Js.Nullable.toOption(theRef);

let getEditor = (state, ~default, ~f) =>
  switch (state.editor^) {
  | None => default
  | Some(editor) => f(editor)
  };

let component = ReasonReact.reducerComponent("Editor_CodeMirror");

let make =
    (
      ~className=?,
      ~value,
      ~options: CodeMirror.EditorConfiguration.t,
      ~setEditor: option(CodeMirror.editor => unit)=?,
      ~onChange=?,
      _children,
    )
    : ReasonReact.component(state, _, unit) => {
  ...component,
  initialState: () => {editor: ref(None), divRef: ref(None), value},
  willReceiveProps: ({state}) =>
    getEditor(state, ~default=state, ~f=editor =>
      {
        ...state,
        value: {
          if (state.value != value
              && value != (editor |. CodeMirror.Editor.getValue())) {
            editor |. CodeMirror.Editor.setValue(value);
          };

          value;
        },
      }
    ),
  didMount: ({state}) =>
    switch (state.divRef^) {
    | None => ()
    | Some(div) =>
      let editor = CodeMirror.make(div, options);
      switch (setEditor) {
      | None => ()
      | Some(setEditor) => setEditor(editor)
      };
      editor |. CodeMirror.Editor.setValue(value);

      editor
      |. CodeMirror.Editor.onChange(
           (editor, diff) => {
             Js.log(diff);
             let currentEditorValue = editor |. CodeMirror.Editor.getValue();
             /* TODO: Figure out why this behaves differently from JS version */
             /* if (currentEditorValue != value) { */
             switch (onChange) {
             | None => ()
             | Some(onChange) => onChange(currentEditorValue)
             };
             /* }; */
           },
         );
      state.editor := Some(editor);
      %bs.raw
      {|window.editor = editor|};
      ();
    },
  render: ({handle, state: _}) =>
    <div ?className ref=(handle(setDivRef)) />,
};
