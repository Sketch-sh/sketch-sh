%bs.raw
{|
/* import "codemirror/mode/mllike/mllike" */
import "./reason-mode.js"
import "codemirror/addon/edit/matchbrackets"
import "codemirror/addon/display/placeholder"
import "codemirror/mode/gfm/gfm"
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
      ~focused,
      ~options: CodeMirror.EditorConfiguration.t,
      ~setEditor: option(CodeMirror.editor => unit)=?,
      ~onBlockUp: option(unit => unit)=?,
      ~onBlockDown: option(unit => unit)=?,
      ~onChange=?,
      ~onFocus=?,
      _children,
    )
    : ReasonReact.component(state, _, unit) => {
  ...component,
  initialState: () => {editor: ref(None), divRef: ref(None), value},
  willReceiveProps: ({state}) =>
    getEditor(
      state,
      ~default=state,
      ~f=editor => {
        switch (focused) {
        | None => ()
        | Some(typ) =>
          open Editor_Types.Block;
          editor |. CodeMirror.Editor.focus;
          switch (typ) {
          | FcTyp_BlockFocusDown =>
            let doc = editor |. CodeMirror.Editor.getDoc;
            doc
            |. CodeMirror.Doc.setCursor(
                 CodeMirror.Position.make(~line=0, ~ch=0, ()),
               );
          | FcTyp_BlockFocusUp =>
            let doc = editor |. CodeMirror.Editor.getDoc;
            let lastLinePlusOne = editor |. CodeMirror.Editor.lineCount;
            doc
            |. CodeMirror.Doc.setCursor(
                 CodeMirror.Position.make(~line=lastLinePlusOne, ~ch=0, ()),
               );
          | FcTyp_EditorFocus => ()
          };
        };
        {
          ...state,
          value: {
            if (state.value != value
                && value != (editor |. CodeMirror.Editor.getValue)) {
              editor |. CodeMirror.Editor.setValue(value);
            };
            value;
          },
        };
      },
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

      switch (onChange) {
      | None => ()
      | Some(onChange) =>
        editor
        |. CodeMirror.Editor.onChange((editor, diff) => {
             Js.log(diff);
             let currentEditorValue = editor |. CodeMirror.Editor.getValue;
             onChange(currentEditorValue);
           })
      };

      switch (onFocus) {
      | None => ()
      | Some(onFocus) =>
        editor |. CodeMirror.Editor.onFocus((_editor, _event) => onFocus())
      };

      switch (onBlockUp, onBlockDown) {
      | (Some(onBlockUp), Some(onBlockDown)) =>
        editor
        |. CodeMirror.Editor.onKeydown((editor, event) => {
             open CodeMirror.Position;
             let doc = editor |. CodeMirror.Editor.getDoc;
             let cursor = doc |. CodeMirror.Doc.getCursor(`head);
             switch (event |. Webapi.Dom.KeyboardEvent.key) {
             | "ArrowUp" when cursor |. lineGet == 0 && cursor |. chGet == 0 =>
               onBlockUp()
             | "ArrowDown" =>
               let lastLine = (editor |. CodeMirror.Editor.lineCount) - 1;
               let lastChar =
                 editor
                 |. CodeMirror.Editor.getLine(lastLine)
                 |. Js.String.length;
               if (lineGet(cursor) == lastLine
                   && lastChar == (cursor |. chGet)) {
                 onBlockDown();
               };
             | _ => ()
             };
           })
      | _ => ()
      };

      state.editor := Some(editor);
      %bs.raw
      {|window.editor = editor|};
      ();
    },
  render: ({handle, state: _}) =>
    <div ?className ref=(handle(setDivRef)) />,
};
