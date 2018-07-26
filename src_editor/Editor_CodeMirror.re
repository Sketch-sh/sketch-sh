%bs.raw
{|
/* import "codemirror/mode/mllike/mllike" */
import "codemirror/addon/edit/matchbrackets"
import "codemirror/mode/rust/rust"
import "codemirror/lib/codemirror.css"

import "./cm-theme.css"
import "./source-editor.css"
import "./app.css" /* <-- Important, no semi */
|};

[%%debugger.chrome];

open Utils;

type state = {
  editor: ref(option(CodeMirror.editor)),
  divRef: ref(option(Dom.element)),
  lineWidgets: ref(list(CodeMirror.LineWidget.t)),
  codeBlockWidgets: array(Editor_CodeBlockTypes.Widget.t),
  value: string,
  firstLineNumber: int,
};

let setDivRef = (theRef, {ReasonReact.state}) =>
  state.divRef := Js.Nullable.toOption(theRef);

let getEditor = (state, ~default, ~f) =>
  switch (state.editor^) {
  | None => default
  | Some(editor) => f(editor)
  };

let component = ReasonReact.reducerComponent("CodeEditor");

let make =
    (
      ~className=?,
      ~value,
      ~options: CodeMirror.EditorConfiguration.t,
      ~firstLineNumber,
      ~widgets,
      ~onChange=?,
      _children,
    )
    : ReasonReact.component(state, _, unit) => {
  ...component,
  initialState: () => {
    editor: ref(None),
    divRef: ref(None),
    lineWidgets: ref([]),
    codeBlockWidgets: widgets,
    value,
    firstLineNumber,
  },
  willReceiveProps: ({state}) =>
    getEditor(state, ~default=state, ~f=editor =>
      {
        ...state,
        value:
          if (state.value != value
              && value != (editor |. CodeMirror.Editor.getValue())) {
            editor |. CodeMirror.Editor.setValue(value);
            value;
          } else {
            state.value;
          },
        firstLineNumber:
          if (state.firstLineNumber != firstLineNumber) {
            editor
            |. CodeMirror.Editor.setOption(
                 "firstLineNumber",
                 firstLineNumber,
               );
            firstLineNumber;
          } else {
            state.firstLineNumber;
          },
        codeBlockWidgets:
          if (widgets != state.codeBlockWidgets) {
            let cachedLineWidgets = state.lineWidgets^;
            cachedLineWidgets
            |. Belt.List.forEachU((. lw)
                 /* TODO: should I remove the domNode ?
                    Potential memory leaks? */
                 => lw |. CodeMirror.LineWidget.clear);

            state.lineWidgets :=
              widgets
              |. Belt.Array.reduceU(
                   [],
                   (. acc, w) => {
                     open Editor_CodeBlockTypes.Widget;
                     open Editor_CodeBlockLineWidget;
                     let newLineWidget =
                       switch (w) {
                       | Lw_Error({content, line}) =>
                         editor
                         |. CodeMirror.Editor.addLineWidget(
                              ~line,
                              ~element=createErrorWidget(content),
                              ~options=
                                CodeMirror.LineWidget.options(
                                  ~coverGutter=true,
                                  ~noHScroll=false,
                                  ~above=false,
                                  ~showIfHidden=false,
                                ),
                            )
                       | Lw_Value({content, line}) =>
                         editor
                         |. CodeMirror.Editor.addLineWidget(
                              ~line,
                              ~element=createValueWidget(content),
                              ~options=
                                CodeMirror.LineWidget.options(
                                  ~coverGutter=false,
                                  ~noHScroll=true,
                                  ~above=false,
                                  ~showIfHidden=false,
                                ),
                            )
                       | Lw_Stdout({content, line}) =>
                         editor
                         |. CodeMirror.Editor.addLineWidget(
                              ~line,
                              ~element=createStdoutWidget(content),
                              ~options=
                                CodeMirror.LineWidget.options(
                                  ~coverGutter=false,
                                  ~noHScroll=false,
                                  ~above=false,
                                  ~showIfHidden=false,
                                ),
                            )
                       };
                     [newLineWidget, ...acc];
                   },
                 );
            widgets;
          } else {
            state.codeBlockWidgets;
          },
      }
    ),
  didMount: ({state}) =>
    switch (state.divRef^) {
    | None => ()
    | Some(div) =>
      let editor = CodeMirror.make(div, options);

      editor |. CodeMirror.Editor.setValue(value);
      editor
      |. CodeMirror.Editor.onChange(
           (editor, diff) => {
             Js.log(diff);
             let currentEditorValue = editor |. CodeMirror.Editor.getValue();

             if (currentEditorValue != value) {
               switch (onChange) {
               | None => ()
               | Some(onChange) => onChange(currentEditorValue)
               };
             };
           },
         );
      state.editor := Some(editor);
      ();
    },
  render: ({handle, state}) => <div ?className ref=(handle(setDivRef)) />,
};
