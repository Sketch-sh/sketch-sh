Modules.require("./reason-mode.js");
Modules.require("codemirror/addon/edit/matchbrackets");
Modules.require("codemirror/addon/edit/closebrackets");
Modules.require("codemirror/addon/comment/comment");
Modules.require("codemirror/addon/display/placeholder");
Modules.require("codemirror/mode/gfm/gfm");
Modules.require("codemirror/mode/mllike/mllike");
Modules.require("codemirror/lib/codemirror.css");
Modules.require("codemirror/addon/fold/foldgutter.css");
Modules.require("./cm-theme.css");

[@react.component]
let make =
    (
      ~className=?,
      ~value,
      ~focused,
      ~editor: option(CodeMirror.editor),
      ~options: CodeMirror.EditorConfiguration.t,
      ~setEditor: CodeMirror.editor => unit,
      ~onBlockUp: option(unit => unit)=?,
      ~onBlockDown: option(unit => unit)=?,
      ~onChange=?,
      ~onFocus: option(unit => unit)=?,
      ~onBlur: option(unit => unit)=?,
      ~onUpdate: option(unit => unit)=?,
    ) => {
  let (didSetEditor, setDidSetEditor) = React.useState(() => false);
  let setEditorCallback = div => {
    switch (div->Js.Nullable.toOption, didSetEditor) {
    | (None, _)
    | (_, true) => ()
    | (Some(div), false) =>
      setDidSetEditor(_ => true);
      let editor = CodeMirror.make(div, options);
      setEditor(editor);
      editor->(CodeMirror.Editor.setValue(value));
      /* This is to prevent Ctrl+Z from clearing the content after mounting */
      editor->CodeMirror.Editor.getDoc->CodeMirror.Doc.clearHistory;
      switch (onChange) {
      | None => ()
      | Some(onChange) =>
        editor->(
                  CodeMirror.Editor.onChange((editor, diff) => {
                    let currentEditorValue =
                      editor->CodeMirror.Editor.getValue;
                    if (value != currentEditorValue) {
                      onChange(currentEditorValue, diff);
                    };
                  })
                )
      };
      switch (onBlur) {
      | None => ()
      | Some(onBlur) =>
        editor->(CodeMirror.Editor.onBlur((_editor, _event) => onBlur()))
      };
      switch (onFocus) {
      | None => ()
      | Some(onFocus) =>
        editor->(CodeMirror.Editor.onFocus((_editor, _event) => onFocus()))
      };
      switch (onUpdate) {
      | None => ()
      | Some(onUpdate) =>
        editor->CodeMirror.Editor.onUpdate(_editor => onUpdate())
      };
      switch (onBlockUp, onBlockDown) {
      | (Some(onBlockUp), Some(onBlockDown)) =>
        editor->(
                  CodeMirror.Editor.onKeydown((editor, event) => {
                    open CodeMirror.Position;
                    let doc = editor->CodeMirror.Editor.getDoc;
                    let cursor = doc->(CodeMirror.Doc.getCursor(`head));
                    switch (event->Webapi.Dom.KeyboardEvent.key) {
                    | "PageUp"
                    | "ArrowUp" when cursor->lineGet == 0 && cursor->chGet == 0 =>
                      onBlockUp()
                    | "PageDown"
                    | "ArrowDown" =>
                      let lastLine = editor->CodeMirror.Editor.lineCount - 1;
                      let lastChar =
                        editor
                        ->(CodeMirror.Editor.getLine(lastLine))
                        ->Js.String.length;
                      if (lineGet(cursor) == lastLine
                          && lastChar == cursor->chGet) {
                        onBlockDown();
                      };
                    | _ => ()
                    };
                  })
                )
      | _ => ()
      };
    };
  };
  React.useEffect2(
    () => {
      switch (focused, editor) {
      | (None, _)
      | (_, None) => ()
      | (Some(typ), Some(editor)) =>
        open Editor_Types.Block;
        editor->CodeMirror.Editor.focus;
        switch (typ) {
        | FcTyp_BlockFocusDown =>
          let doc = editor->CodeMirror.Editor.getDoc;
          doc->(
                 CodeMirror.Doc.setCursor(
                   CodeMirror.Position.make(~line=0, ~ch=0, ()),
                 )
               );
        | FcTyp_BlockFocusUp =>
          let doc = editor->CodeMirror.Editor.getDoc;
          let lastLinePlusOne = editor->CodeMirror.Editor.lineCount;
          doc->(
                 CodeMirror.Doc.setCursor(
                   CodeMirror.Position.make(~line=lastLinePlusOne, ~ch=0, ()),
                 )
               );
        | FcTyp_BlockNew
        | FcTyp_BlockExecuteAndFocusNextBlock
        | FcTyp_EditorFocus => ()
        };
      };
      None;
    },
    (editor, focused),
  );
  React.useEffect2(
    () => {
      switch (editor) {
      | None => ()
      | Some(editor) =>
        if (value != editor->CodeMirror.Editor.getValue) {
          editor->(CodeMirror.Editor.setValue(value));
        }
      };
      None;
    },
    (editor, value),
  );
  <div ?className ref={ReactDOMRe.Ref.callbackDomRef(setEditorCallback)} />;
};
