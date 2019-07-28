open Utils;
open Edit_state;

module S = {
  open Css;
  open Ds_unit;

  let border_color = Color.light_3;

  let header =
    [
      borderBottom(BorderWidth.bw1, `solid, Color.light_3),
      paddingBottom(Space.s2),
      paddingRight(Space.s4),
      paddingLeft(Space.s4),
      marginTop(Space.s2),
      height(Space.s10),
      zIndex(1),
      display(`flex),
      alignItems(`center),
    ]
    ->style;

  let main =
    [
      display(`flex),
      flex(`num(1.)),
      overflow(`hidden),
      unsafe("flexFlow", "nowrap"),
      zIndex(0),
    ]
    ->style;

  let sidebar =
    [
      display(`flex),
      flexDirection(`column),
      maxWidth(`percent(50.)),
      minWidth(`px(130)),
      width(`px(250)),
      position(`relative),
      zIndex(5),
    ]
    ->style;

  let editor_container = [display(`flex), flex(`num(1.))]->style;

  let editor_wrap =
    [
      display(`flex),
      flexDirection(`column),
      flex(`num(1.)),
      overflowX(`auto),
      overflowY(`hidden),
      borderLeft(BorderWidth.bw1, `solid, border_color),
    ]
    ->style;

  let preview_container =
    [
      position(`relative),
      width(`vw(40.)),
      minWidth(`px(200)),
      maxWidth(`percent(75.)),
      minHeight(`px(100)),
      borderLeft(BorderWidth.bw1, `solid, border_color),
      unsafe("WebkitOverflowScrolling", "touch"),
    ]
    ->style;

  let frame =
    [
      position(`absolute),
      top(`zero),
      right(`zero),
      bottom(`zero),
      left(`zero),
      borderStyle(`none),
      width(`percent(100.)),
      height(`percent(100.)),
    ]
    ->style;
};

let default_value = {|[@bs.deriving jsConverter]
type t =
  | [@bs.as 1] Hint
  | [@bs.as 2] Info
  | [@bs.as 4] Warning
  | [@bs.as 8] Error;

let a = tToJs(Hint);

Js.log(a);

[@bs.deriving jsConverter]
type t =
  | [@bs.as 1] Hint
  | [@bs.as 2] Info
  | [@bs.as 4] Warning
  | [@bs.as 8] Error;

let a = tToJs(Hint);

Js.log(a);|};

let initial_files = {
  Belt.Map.String.empty
  ->Belt.Map.String.set("index.re", Edit_state.make_file(default_value))
  ->Belt.Map.String.set(
      "second_file.re",
      Edit_state.make_file(
        "// Sketch doesn't support multiple files yet.\n// This is just for design mockup\n",
      ),
    );
};

[@react.component]
let make = () => {
  let (state, send) =
    ReactUpdate.useReducer(
      {files: initial_files, active_file: "index.re"},
      reducer,
    );

  <>
    <header className=S.header> "Sketch.sh"->str </header>
    <main className=S.main>
      <div className=S.editor_container>
        <section className=S.sidebar>
          <Edit_treeview
            files={state.files}
            active_file={state.active_file}
            on_file_click={filename => send(File_open(filename))}
          />
        </section>
        <section className=S.editor_wrap>
          {switch (state.files->Belt.Map.String.get(state.active_file)) {
           | None => "Nothing to show here"->str
           | Some({code, errors, warnings, compiled}) =>
             <Edit_editor
               value=code
               errors
               warnings
               ?compiled
               onChange={new_value =>
                 send(File_update(state.active_file, new_value))
               }
             />
           }}
        </section>
      </div>
      <div className=S.preview_container>
        <iframe
          id="frame"
          className=S.frame
          src="/container.html"
          sandbox="allow-modals allow-scripts allow-popups allow-forms allow-same-origin"
        />
      </div>
    </main>
  </>;
};
