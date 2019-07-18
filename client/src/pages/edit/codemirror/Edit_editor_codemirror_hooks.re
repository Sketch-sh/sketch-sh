open Edit_state_native;
module Execute = Engine_native.Types.Execute;
module Parse = Engine_native.Types.Parse;
module Error = Engine_native.Types.Error;

let get_editor = (editor_ref, cb) => {
  switch (editor_ref->React.Ref.current) {
  | None => ()
  | Some(editor) => cb(editor)
  };
};

let use_exec_result = (~exec_msg, ~editor_ref) => {
  React.useEffect1(
    () => {
      let markers = ref([]);
      let value_view = ref([]);
      let stdout_view = ref([]);

      exec_msg->Belt.Map.Int.forEach(
        (
          _id,
          {
            Execute.exec_loc: _,
            exec_phr_kind: _,
            exec_content,
            exec_warning,
            exec_stdout,
          },
        ) => {
        switch (exec_content) {
        | Execute.Exec_phr_exn(err) =>
          markers := [Codemirror_helpers.make_error_marker(err), ...markers^]
        | _ => ()
        };

        markers :=
          Belt.List.concat(
            exec_warning->Belt.List.map(
              Codemirror_helpers.make_warning_marker,
            ),
            markers^,
          );
        ();
      });

      get_editor(editor_ref, editor =>
        CodeMirror.Editor.updateLinting(editor, (markers^)->Belt.List.toArray)
      );

      None;
    },
    [|exec_msg|],
  );
};

let use_parse_error = (~parse_error, ~editor_ref) => {
  React.useEffect1(
    () => {
      get_editor(
        editor_ref,
        editor => {
          open! CodeMirror;

          let lints =
            switch (parse_error) {
            | None => [||]
            | Some(error) => [|Codemirror_helpers.make_error_marker(error)|]
            };
          CodeMirror.Editor.updateLinting(editor, lints);
        },
      );

      None;
    },
    [|parse_error|],
  );
};
