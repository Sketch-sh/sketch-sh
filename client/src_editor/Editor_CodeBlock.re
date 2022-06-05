open Editor_Types;

let langToMode =
  fun
  | RE => "reason"
  | ML => "mllike";

[@react.component]
let make =
    (
      ~onFocus=?,
      ~onBlur=?,
      ~onBlockUp=?,
      ~onBlockDown=?,
      ~onUpdate=?,
      ~readOnly=?,
      ~viewportMargin=infinity,
      ~lang=ML,
      ~value,
      ~focused,
      ~firstLineNumber,
      ~widgets,
      ~onChange,
    ) => {
  let (editor, setEditor) = React.useState(() => None);
  let (cachedLineWidgets, setCachedLineWidgets) = React.useState(() => []);

  React.useEffect1(
    () => {
      switch (editor) {
      | None => ()
      | Some(editor) =>
        editor->(
                  CodeMirror.Editor.setOption(
                    "firstLineNumber",
                    firstLineNumber,
                  )
                )
      };
      None;
    },
    [|firstLineNumber|],
  );

  React.useEffect1(
    () => {
      switch (editor) {
      | None => ()
      | Some(editor) =>
        editor->(CodeMirror.Editor.setOption("mode", lang->langToMode))
      };
      None;
    },
    [|lang|],
  );

  React.useEffect1(
    () => {
      switch (editor) {
      | None => ()
      | Some(editor) =>
        editor->CodeMirror.Editor.operation((.) => {
          cachedLineWidgets->(
                               Belt.List.forEachU((. lw)
                                 /* TODO: should I remove the domNode ?
                                    Potential memory leaks? */
                                 => lw->CodeMirror.LineWidget.clear)
                             );

          setCachedLineWidgets(_ =>
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
                     )
          );
        })
      };
      None;
    },
    [|widgets|],
  );

  <Editor_CodeMirror
    value
    focused
    onChange
    ?onFocus
    ?onBlur
    ?onBlockUp
    ?onBlockDown
    ?onUpdate
    setEditor={editor => setEditor(_ => Some(editor))}
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
      ~extraKeys=
        {let key = Js.Dict.empty();

         Js.Dict.set(key, "Tab", "indentMore");
         Js.Dict.set(key, "Shift-Tab", "indentLess");
         Js.Dict.set(key, "Cmd-/", "toggleComment");
         Js.Dict.set(key, "Ctrl-/", "toggleComment");
         key},
      (),
    )}
  />;
};
