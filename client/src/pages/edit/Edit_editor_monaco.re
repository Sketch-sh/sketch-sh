module S: {
  let gutter_executed: string;
  let gutter_parsed: string;
} = {
  open Css;
  open Ds_unit;

  let gutter_base = [
    unsafe("width", "5px !important"),
    marginLeft(`px(5)),
  ];
  let gutter_executed = [background(Color.light_2), ...gutter_base]->style;
  let gutter_parsed =
    [background(Color.brand_light2), ...gutter_base]->style;
};

let get_monaco = (monaco_ref, cb) => {
  switch (monaco_ref->React.Ref.current->Js.Nullable.toOption) {
  | None => ()
  | Some(monaco) => cb(monaco)
  };
};

open Edit_state_native;

let parse = Debouncer.make(((code, send)) => send(Out_parse(code)));
[@react.component]
let make = (~value, ~onChange) => {
  let monaco_ref = React.useRef(Js.Nullable.null);
  let (deco, set_deco) = React.useState(() => [||]);

  let (ntv_state, ntv_send) =
    ReactUpdate.useReducer(
      {parse_error: None, parse_success: [||], last_executed_line: Some(1)},
      reducer,
    );
  /* Handling gutter status */
  React.useEffect2(
    () => {
      open MonacoEditor;
      let {executed, parsed_and_executable} =
        derive_execution_status(
          ntv_state.parse_success,
          ntv_state.last_executed_line,
        );

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
                ~linesDecorationsClassName=S.gutter_executed,
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
                ~linesDecorationsClassName=S.gutter_parsed,
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
    (ntv_state.parse_success, ntv_state.last_executed_line),
  );

  /* Parsing the statements on value changes */
  React.useEffect1(
    () => {
      parse((value, ntv_send));
      None;
    },
    [|value|],
  );

  /* Handling error markers */
  React.useEffect1(
    () => {
      open MonacoEditor;
      let markers =
        switch (ntv_state.parse_error) {
        | None => [||]
        | Some(err) =>
          let (startLineNumber, startColumn, endLineNumber, endColumn) =
            Location.Monaco.to_monaco(err##loc);

          let marker =
            IMarkerData.make(
              ~severity=MarkerSeverity.Error,
              ~message=err##message,
              ~startLineNumber,
              ~startColumn,
              ~endLineNumber,
              ~endColumn,
              (),
            );
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
    [|ntv_state.parse_error|],
  );

  <ReactMonacoEditor
    defaultValue=value
    ref=monaco_ref
    language="reason"
    editorWillMount={monaco =>
      monaco##languages##register(
        MonacoEditor.Languages.languageExtensionPoint(~id="reason", ()),
      )
    }
    onChange={(new_value, event) => {
      ntv_send(
        Editor_changed(
          event##changes
          ->Belt.Array.map(changes =>
              Location.Monaco.from_monaco(changes##range)
            ),
        ),
      );
      onChange(new_value);
    }}
  />;
};
