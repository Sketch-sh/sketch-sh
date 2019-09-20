let get_editor = (editor_ref, cb) => {
  switch (editor_ref->React.Ref.current) {
  | None => ()
  | Some(editor) => cb(editor)
  };
};

let update_markers = (~errors, ~warnings, ~editor_ref) => {
  React.useEffect2(
    () => {
      get_editor(
        editor_ref,
        editor => {
          open! CodeMirror;

          let lints = [||];
          errors->Belt.Array.forEach(error => {
            let _ =
              error
              ->Codemirror_helpers.make_error_marker
              ->Js.Array.push(lints);
            ();
          });
          warnings->Belt.Array.forEach(error => {
            let _ =
              error
              ->Codemirror_helpers.make_warning_marker
              ->Js.Array.push(lints);
            ();
          });

          CodeMirror.Editor.updateLinting(editor, lints);
        },
      );

      None;
    },
    (errors, warnings),
  );
};
