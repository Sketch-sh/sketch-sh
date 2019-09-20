open Edit_state_native;
module Execute = Engine_native.Types.Execute;
module Parse = Engine_native.Types.Parse;

let get_monaco = (monaco_ref, cb) => {
  switch (monaco_ref->React.Ref.current->Js.Nullable.toOption) {
  | None => ()
  | Some(monaco) => cb(monaco)
  };
};

let use_gutter_status =
    (
      ~parse_success,
      ~last_executed_line,
      ~monaco_ref,
      ~class_gutter_executed,
      ~class_gutter_parsed,
    ) => {
  let (deco, set_deco) = React.useState(() => [||]);

  React.useEffect2(
    () => {
      open MonacoEditor;
      let {executed, parsed_and_executable} =
        derive_execution_status(parse_success, last_executed_line);

      let to_range = pair => {
        IRange.make(
          ~startLineNumber=fst(pair) + 1,
          ~startColumn=1,
          ~endLineNumber=snd(pair) + 1,
          ~endColumn=1,
        );
      };
      let executed_deco =
        executed->Belt.Option.map(executed =>
          IModelDeltaDecoration.make(
            ~range=to_range(executed),
            ~options=
              IModelDecorationOptions.make(
                ~isWholeLine=true,
                ~linesDecorationsClassName=class_gutter_executed,
                (),
              ),
          )
        );

      let parsed_deco =
        parsed_and_executable->Belt.Option.map(parsed =>
          IModelDeltaDecoration.make(
            ~range=to_range(parsed),
            ~options=
              IModelDecorationOptions.make(
                ~isWholeLine=true,
                ~linesDecorationsClassName=class_gutter_parsed,
                (),
              ),
          )
        );

      let decorations =
        [|executed_deco, parsed_deco|]->Belt.Array.keepMap(a => a);

      get_monaco(
        monaco_ref,
        react_monaco => {
          let new_deco =
            react_monaco##editor##deltaDecorations(deco, decorations);
          set_deco(_ => new_deco);
        },
      );

      None;
    },
    (parse_success, last_executed_line),
  );
};

let use_exec_result = (~exec_msg, ~monaco_ref) => {
  React.useEffect1(
    () => {
      open MonacoEditor;

      let markers = ref([]);
      let _value_view = ref([]);
      let _stdout_view = ref([]);

      exec_msg->Belt.Map.Int.forEach(
        (
          _id,
          {
            Execute.exec_loc: _,
            exec_phr_kind: _,
            exec_content,
            exec_warning,
            exec_stdout: _,
          },
        ) => {
        switch (exec_content) {
        | Execute.Exec_phr_exn(err) =>
          markers := [Monaco_helpers.make_error_marker(err), ...markers^]
        | _ => ()
        };

        markers :=
          Belt.List.concat(
            exec_warning->Belt.List.map(Monaco_helpers.make_warning_marker),
            markers^,
          );
        ();
      });

      get_monaco(
        monaco_ref,
        react_monaco => {
          let model = react_monaco##editor##getModel();

          monaco##editor##setModelMarkers(
            model,
            "refmt",
            (markers^)->Belt.List.toArray,
          );
        },
      );

      None;
    },
    [|exec_msg|],
  );
};

let use_parse_error = (~parse_error, ~monaco_ref) => {
  React.useEffect1(
    () => {
      open MonacoEditor;
      let markers =
        switch (parse_error) {
        | None => [||]
        | Some(err) =>
          let marker = Monaco_helpers.make_error_marker(err);
          [|marker|];
        };

      get_monaco(
        monaco_ref,
        react_monaco => {
          let model = react_monaco##editor##getModel();

          monaco##editor##setModelMarkers(model, "refmt", markers);
        },
      );

      None;
    },
    [|parse_error|],
  );
};
