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
      ~readOnly=?,
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
            ->(
                CodeMirror.Editor.setOption("firstLineNumber", firstLineNumber)
              );
          };
          firstLineNumber;
        },
        codeBlockWidgets: {
          if (widgets != state.codeBlockWidgets) {
            let cachedLineWidgets = state.lineWidgets^;

            cachedLineWidgets
            ->(
                Belt.List.forEachU((. lw)
                  /* TODO: should I remove the domNode ?
                     Potential memory leaks? */
                  => lw->CodeMirror.LineWidget.clear)
              );

            state.lineWidgets :=
              widgets
              ->(
                  Belt.Array.reduceU(
                    [],
                    (. acc, w) => {
                      open Editor_CodeBlockTypes.Widget;
                      open Editor_LineWidget;
                      let {lw_line: line, lw_data} = w;
                      let newLineWidget =
                        switch (lw_data) {
                        | Lw_Error(content) =>
                          editor
                          ->(
                              CodeMirror.Editor.addLineWidget(
                                ~line,
                                ~element=createErrorWidget(content),
                                ~options=
                                  CodeMirror.LineWidget.options(
                                    ~coverGutter=false,
                                    ~noHScroll=false,
                                    ~above=false,
                                    ~showIfHidden=false,
                                    ~handleMouseEvents=true,
                                  ),
                              )
                            )
                        | Lw_Warning(content) =>
                          editor
                          ->(
                              CodeMirror.Editor.addLineWidget(
                                ~line,
                                ~element=createWarningWidget(content),
                                ~options=
                                  CodeMirror.LineWidget.options(
                                    ~coverGutter=false,
                                    ~noHScroll=false,
                                    ~above=false,
                                    ~showIfHidden=false,
                                    ~handleMouseEvents=true,
                                  ),
                              )
                            )
                        | Lw_Value(content) =>
                          editor
                          ->(
                              CodeMirror.Editor.addLineWidget(
                                ~line,
                                ~element=createValueWidget(content),
                                ~options=
                                  CodeMirror.LineWidget.options(
                                    ~coverGutter=false,
                                    ~noHScroll=false,
                                    ~above=false,
                                    ~showIfHidden=false,
                                    ~handleMouseEvents=true,
                                  ),
                              )
                            )
                        | Lw_Stdout(content) =>
                          editor
                          ->(
                              CodeMirror.Editor.addLineWidget(
                                ~line,
                                ~element=createStdoutWidget(content),
                                ~options=
                                  CodeMirror.LineWidget.options(
                                    ~coverGutter=false,
                                    ~noHScroll=false,
                                    ~above=false,
                                    ~showIfHidden=false,
                                    ~handleMouseEvents=true,
                                  ),
                              )
                            )
                        };
                      [newLineWidget, ...acc];
                    },
                  )
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
          ~theme=Config.cmTheme,
          ~lineNumbers=true,
          ~styleActiveLine=true,
          ~viewportMargin=infinity,
          ~matchBrackets=true,
          ~lineWrapping=true,
          ~firstLineNumber,
          ~tabSize=2,
          ~readOnly?,
          ~extraKeys={
            let key = Js.Dict.empty();

            Js.Dict.set(key, "Tab", "indentMore");
            Js.Dict.set(key, "Shift-Tab", "indentLess");
            key;
          },
          (),
        )
      )
    />,
};
