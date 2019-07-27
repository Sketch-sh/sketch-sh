%raw
{|
  require("./reason-mode.js");
  require("codemirror/addon/selection/active-line");
  require("./lint.js");
  require("codemirror/lib/codemirror.css");
  require("codemirror/addon/lint/lint.css");
|};

module S = {
  open Css;
  open Ds_unit;

  global(".CodeMirror", [width(`percent(100.)), height(`percent(100.))]);
  global(
    ".CodeMirror-activeline-background",
    [backgroundColor(Color.light_3)],
  );
  global(".CodeMirror-activeline-gutter", [backgroundColor(Color.light_4)]);
  global(".CodeMirror-linenumber", [color(Color.light)]);
  global(
    ".CodeMirror-gutters",
    [
      backgroundColor(Color.brand_invert),
      paddingRight(Space.s1),
      borderRightWidth(`zero),
    ],
  );

  let wrap =
    [
      display(`flex),
      flexDirection(`row),
      flex(`num(1.)),
      width(`percent(100.)),
      height(`percent(100.)),
    ]
    ->style;
  let editor =
    [width(`percent(100.)), height(`percent(100.)), fontSize(`px(14))]
    ->style;
  let view =
    [
      flex3(~grow=0., ~shrink=0., ~basis=`px(350)),
      paddingLeft(Space.s2),
      paddingRight(Space.s2),
      backgroundColor(Color.light_4),
    ]
    ->style;
};

open Edit_state_native;

let parse = Debouncer.make(((code, send)) => send(Out_parse(code)));

let editor_config =
  CodeMirror.EditorConfiguration.make(
    ~mode="reason",
    ~lineNumbers=true,
    ~viewportMargin=infinity,
    ~firstLineNumber=0,
    ~lineWrapping=true,
    ~styleActiveLine=true,
    ~gutters=[|"CodeMirror-lint-markers", "CodeMirror-linenumbers"|],
    ~lint=true,
    (),
  );

[@react.component]
let make = (~value, ~onChange) => {
  let editor_ref = React.useRef(None);

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
  Edit_editor_codemirror_hooks.use_exec_result(
    ~exec_msg=ntv_state.exec_msg,
    ~editor_ref,
  );
  /* Handling parsing error markers */
  // Edit_editor_codemirror_hooks.use_parse_error(
  //   ~parse_error=ntv_state.parse_error,
  //   ~editor_ref,
  // );

  <div className=S.wrap>
    <Ds.Button
      onClick={_ =>
        switch (ntv_state.parse_success) {
        | [] => ()
        | [hd, ..._xs] => ntv_send(Execute(hd.id))
        }
      }>
      "Execute all"->React.string
    </Ds.Button>
    <ReactCodeMirror.Controlled
      value
      options=editor_config
      onBeforeChange={(_editor, _changes, value) => {
        onChange(value);
        parse((value, ntv_send));
      }}
      editorDidMount={editor => {
        %raw
        {|window.editor = editor|};
        editor_ref->React.Ref.setCurrent(Some(editor));
      }}
      className=S.editor
    />
  </div>;
  // <div className=S.view> "data"->React.string </div>
};
