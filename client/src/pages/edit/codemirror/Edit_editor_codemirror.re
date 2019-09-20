%raw
{|
  require("./reason-mode.js");
  require("codemirror/mode/javascript/javascript");
  require("codemirror/addon/selection/active-line");
  require("./lint.js");
  require("codemirror/lib/codemirror.css");
  require("codemirror/addon/lint/lint.css");
|};

module S = {
  open Css;
  open Ds_unit;

  let editor =
    [
      width(`percent(100.)),
      height(`percent(100.)),
      fontSize(`px(14)),
      overflow(`hidden),
    ]
    ->style;

  global(
    ".react-codemirror2 .CodeMirror",
    [width(`percent(100.)), height(`percent(100.))],
  );
  global(
    ".react-codemirror2 .CodeMirror-activeline-background",
    [backgroundColor(Color.light_3)],
  );
  global(
    ".react-codemirror2 .CodeMirror-activeline-gutter",
    [backgroundColor(Color.light_4)],
  );
  global(".react-codemirror2 .CodeMirror-linenumber", [color(Color.light)]);
  global(
    ".react-codemirror2 .CodeMirror-gutters",
    [
      backgroundColor(Color.brand_invert),
      paddingRight(Space.s1),
      borderRightWidth(`zero),
    ],
  );
};

let base_config =
  CodeMirror.EditorConfiguration.make(
    ~lineNumbers=true,
    ~viewportMargin=infinity,
    ~firstLineNumber=1,
    ~lineWrapping=true,
    ~styleActiveLine=true,
    ~gutters=[|"CodeMirror-lint-markers", "CodeMirror-linenumbers"|],
    ~lint=true,
  );

let reason_config = base_config(~mode="reason", ());
let compile_config = base_config(~mode="javascript", ~readOnly=true, ());

// open Utils;

[@react.component]
let make = (~value, ~onChange, ~errors, ~warnings, ~compiled=?) => {
  // let (show_compiled, set_show_compiled) = React.useState(() => false);
  let editor_ref = React.useRef(None);

  Edit_editor_codemirror_hooks.update_markers(
    ~errors,
    ~warnings,
    ~editor_ref,
  );

  <Reflex.Container orientation="horizontal">
    <Reflex.Element>
      <ReactCodeMirror.Controlled
        value
        options=reason_config
        onBeforeChange={(_editor, _changes, value) => onChange(value)}
        editorDidMount={editor => {
          %raw
          {|window.editor = editor|};
          editor_ref->React.Ref.setCurrent(Some(editor));
        }}
        className=S.editor
      />
    </Reflex.Element>
    <Reflex.Splitter className=Ds.splitter_className />
    <Reflex.Element>
      <ReactCodeMirror.Controlled
        value={compiled->Belt.Option.getWithDefault("")}
        options=compile_config
        className=S.editor
      />
    </Reflex.Element>
  </Reflex.Container>;
};
