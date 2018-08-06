type state = {
  editor: ref(option(CodeMirror.editor)),
  lineWidgets: ref(list(CodeMirror.LineWidget.t)),
  codeBlockWidgets: array(Editor_CodeBlockTypes.Widget.t),
  firstLineNumber: int,
};

let component = ReasonReact.reducerComponent("Editor_CodeBlock");

let getEditor = (state, ~default, ~f) =>
  switch (state.editor^) {
  | None => default
  | Some(editor) => f(editor)
  };

let make =
    (
      ~value,
      ~focused,
      ~firstLineNumber,
      ~widgets,
      ~onChange,
      ~onFocus=?,
      ~onBlur=?,
      ~onBlockUp=?,
      ~onBlockDown=?,
      ~onExecute,
      _children,
    )
    : ReasonReact.component(state, _, unit) => {
  ...component,
  initialState: () => {
    editor: ref(None),
    lineWidgets: ref([]),
    codeBlockWidgets: widgets,
    firstLineNumber,
  },
  willReceiveProps: ({state}) =>
    getEditor(state, ~default=state, ~f=editor =>
      {
        ...state,
        firstLineNumber: {
          if (state.firstLineNumber != firstLineNumber) {
            editor
            |. CodeMirror.Editor.setOption("firstLineNumber", firstLineNumber);
          };
          firstLineNumber;
        },
        codeBlockWidgets: {
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
                     let {lw_line: line, lw_data} = w;
                     let newLineWidget =
                       switch (lw_data) {
                       | Lw_Error(content) =>
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
                       | Lw_Warning(content) =>
                         editor
                         |. CodeMirror.Editor.addLineWidget(
                              ~line,
                              ~element=createWarningWidget(content),
                              ~options=
                                CodeMirror.LineWidget.options(
                                  ~coverGutter=true,
                                  ~noHScroll=false,
                                  ~above=false,
                                  ~showIfHidden=false,
                                ),
                            )
                       | Lw_Value(content) =>
                         editor
                         |. CodeMirror.Editor.addLineWidget(
                              ~line,
                              ~element=createValueWidget(content),
                              ~options=
                                CodeMirror.LineWidget.options(
                                  ~coverGutter=false,
                                  ~noHScroll=false,
                                  ~above=false,
                                  ~showIfHidden=false,
                                ),
                            )
                       | Lw_Stdout(content) =>
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
          };
          widgets;
        },
      }
    ),
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
          ();
        }
      )
      options=(
        CodeMirror.EditorConfiguration.make(
          ~mode="reason",
          ~theme="rtop-light",
          ~lineNumbers=true,
          ~viewportMargin=infinity,
          ~matchBrackets=true,
          ~lineWrapping=true,
          ~firstLineNumber,
          ~extraKeys={
            let key = Js.Dict.empty();
            key
            |. Js.Dict.set("Tab", cm => {
                 let spaces =
                   String.make(
                     cm |. CodeMirror.Editor.GetOption.indentUnit,
                     ' ',
                   );
                 cm |. CodeMirror.Editor.replaceSelection(spaces);
               });
            key |. Js.Dict.set("Shift-Enter", _cm => onExecute());
            key;
          },
          (),
        )
      )
    />,
};
