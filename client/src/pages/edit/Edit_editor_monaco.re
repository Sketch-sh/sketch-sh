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

open Edit_state_native;

let parse = Debouncer.make(((code, send)) => send(Out_parse(code)));

[@react.component]
let make = (~value, ~onChange) => {
  let monaco_ref = React.useRef(Js.Nullable.null);

  let (ntv_state, ntv_send) =
    ReactUpdate.useReducer(
      {
        parse_error: None,
        parse_success: [],
        last_executed_line: Some(1),
        exec_msg: Belt.Map.Int.empty,
      },
      reducer,
    );

  /* Parsing the statements on value changes */
  React.useEffect1(
    () => {
      parse((value, ntv_send));
      None;
    },
    [|value|],
  );

  /* Handling display of execution result */
  Edit_editor_monaco_hooks.use_exec_result(
    ~exec_msg=ntv_state.exec_msg,
    ~monaco_ref,
  );

  /* Handling parsing error markers */
  Edit_editor_monaco_hooks.use_parse_error(
    ~parse_error=ntv_state.parse_error,
    ~monaco_ref,
  );

  /* Handling gutter status */
  Edit_editor_monaco_hooks.use_gutter_status(
    ~parse_success=ntv_state.parse_success,
    ~last_executed_line=ntv_state.last_executed_line,
    ~monaco_ref,
    ~class_gutter_executed=S.gutter_executed,
    ~class_gutter_parsed=S.gutter_parsed,
  );

  <>
    <Ds.Button
      onClick={_ =>
        switch (ntv_state.parse_success) {
        | [] => ()
        | [hd, ..._xs] => ntv_send(Execute(hd.id))
        }
      }>
      "Execute all"->React.string
    </Ds.Button>
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
                changes##range->Loc.Monaco.from_monaco
              ),
          ),
        );
        onChange(new_value);
      }}
    />
  </>;
};
