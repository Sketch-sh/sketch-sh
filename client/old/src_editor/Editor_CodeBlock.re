open Editor_Types;
type state = {
  editor: ref(option(CodeMirror.editor)),
  lineWidgets: ref(list(CodeMirror.LineWidget.t)),
  codeBlockWidgets: array(Widget.t),
  firstLineNumber: int,
  lang,
};

let component = ReasonReact.reducerComponent("Editor_CodeBlock");

let getEditor = (state, ~default, ~f) =>
  switch (state.editor^) {
  | None => default
  | Some(editor) => f(editor)
  };

let langToMode =
  fun
  | RE => "reason"
  | ML => "mllike";

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
      ~onUpdate=?,
      ~readOnly=?,
      ~viewportMargin=infinity,
      ~lang=RE,
      _children,
    )
    : ReasonReact.component(state, _, unit) => {
  ...component,
  initialState: () => {
    editor: ref(None),
    lineWidgets: ref([]),
    codeBlockWidgets: widgets,
    firstLineNumber,
    lang,
  },
  willReceiveProps: ({state}) =>
    getEditor(state, ~default=state, ~f=editor =>
      {
        ...state,
        firstLineNumber: {
          if (state.firstLineNumber != firstLineNumber) {
            editor->(
                      CodeMirror.Editor.setOption(
                        "firstLineNumber",
                        firstLineNumber,
                      )
                    );
          };
          firstLineNumber;
        },
        lang: {
          if (state.lang != lang) {
            editor->(CodeMirror.Editor.setOption("mode", lang->langToMode));
          };
          lang;
        },
        codeBlockWidgets: {
          if (widgets != state.codeBlockWidgets) {
            let cachedLineWidgets = state.lineWidgets^;

            editor->CodeMirror.Editor.operation((.) => {
              cachedLineWidgets->(
                                   Belt.List.forEachU((. lw)
                                     /* TODO: should I remove the domNode ?
                                        Potential memory leaks? */
                                     => lw->CodeMirror.LineWidget.clear)
                                 );

              state.lineWidgets :=
                widgets->(
                           Belt.Array.reduceU(
                             [],
                             (. acc, w) => {
                               open Editor_Types.Widget;
                               open Editor_LineWidget;
                               let {lw_line: line, lw_data} = w;
                               let newLineWidget =
                                 switch (lw_data) {
                                 | Lw_Error(content) =>
                                   editor->(
                                             CodeMirror.Editor.addLineWidget(
                                               ~line,
                                               ~element=
                                                 createErrorWidget(content),
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
                                   editor->(
                                             CodeMirror.Editor.addLineWidget(
                                               ~line,
                                               ~element=
                                                 createWarningWidget(content),
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
                                   let (element, setCallback) =
                                     createValueWidget(content);
                                   let newLineWidget =
                                     editor->(
                                               CodeMirror.Editor.addLineWidget(
                                                 ~line,
                                                 ~element,
                                                 ~options=
                                                   CodeMirror.LineWidget.options(
                                                     ~coverGutter=false,
                                                     ~noHScroll=false,
                                                     ~above=false,
                                                     ~showIfHidden=false,
                                                     ~handleMouseEvents=true,
                                                   ),
                                               )
                                             );
                                   setCallback(() =>
                                     newLineWidget->CodeMirror.LineWidget.changed
                                   );
                                   newLineWidget;
                                 | Lw_Stdout(content) =>
                                   editor->(
                                             CodeMirror.Editor.addLineWidget(
                                               ~line,
                                               ~element=
                                                 createStdoutWidget(content),
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
            });
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
      ?onUpdate
      setEditor={editor => state.editor := Some(editor)}
      options={CodeMirror.EditorConfiguration.make(
        ~mode=lang->langToMode,
        ~theme=Config.cmTheme,
        ~lineNumbers=true,
        ~styleActiveLine=true,
        ~viewportMargin,
        ~matchBrackets=true,
        ~autoCloseBrackets=true,
        ~lineWrapping=true,
        ~smartIndent=false,
        ~firstLineNumber,
        ~tabSize=2,
        ~readOnly?,
        ~extraKeys={
          let key = Js.Dict.empty();

          Js.Dict.set(key, "Tab", "indentMore");
          Js.Dict.set(key, "Shift-Tab", "indentLess");
          Js.Dict.set(key, "Cmd-/", "toggleComment");
          Js.Dict.set(key, "Ctrl-/", "toggleComment");
          key;
        },
        (),
      )}
    />,
};
