Modules.require("../src/App.css");
Modules.require("./Embed.css");

[@bs.val] external atob: string => string = "";
[@bs.val] external btoa: string => string = "";

let getScrollHeight: unit => float =
  () => [%bs.raw {|document.body.scrollHeight|}];

let postMessage: Js.t('a) => unit = [%raw
  {| function (message) {
     window.parent && window.parent.postMessage(message, "*")
  } |}
];
type state = {
  value: string,
  lang: Editor_Types.lang,
  widgets: array(Editor_Types.Widget.t),
  scrollHeight: ref(float),
};

type action =
  | UpdateValue(string)
  | UpdateWidgets(array(Editor_Types.Widget.t))
  | EditorUpdate
  | Run;

let component = ReasonReact.reducerComponent("Embed");

let make = _children => {
  ...component,
  didMount: ({state: _, send}) => send(Run),
  initialState: () => {
    let search = [%bs.raw "window.location.search"];
    let params = URLSearchParams.make(search);
    let value = params->URLSearchParams.get("value");
    let value =
      switch (value) {
      | None => ""
      | Some(value) => atob(value)
      };

    let lang =
      params->URLSearchParams.get("lang")->Belt.Option.getWithDefault("RE");
    let lang =
      switch (lang->Editor_Types.stringToLang) {
      | value => value
      | exception _ => RE
      };
    {value, lang, widgets: [||], scrollHeight: ref(getScrollHeight())};
  },
  reducer: (action, state) =>
    switch (action) {
    | UpdateValue(value) => ReasonReact.Update({...state, value})
    | UpdateWidgets(widgets) => ReasonReact.Update({...state, widgets})
    | EditorUpdate =>
      ReasonReact.SideEffects(
        (
          ({state}) => {
            let newScrollHeight = getScrollHeight();

            if (newScrollHeight != state.scrollHeight^) {
              let src: string = [%bs.raw "window.location.href"];
              postMessage({
                "type": "iframe.resize",
                "src": src,
                "height": newScrollHeight,
              });
              state.scrollHeight := newScrollHeight;
            };
          }
        ),
      )
    | Run =>
      ReasonReact.SideEffects(
        (
          ({state, send, onUnmount}) => {
            let cancelToken =
              Toplevel_Consumer.executeEmbed(
                ~lang=state.lang,
                ~code=state.value,
                fun
                | Belt.Result.Ok(result) => {
                    let widgets =
                      Editor_Blocks_Utils.executeResultToWidget(result);
                    send(UpdateWidgets(widgets));
                  }
                | Belt.Result.Error(message) => Js.log(message),
              );
            onUnmount(() => Toplevel_Consumer.cancel(cancelToken));
          }
        ),
      )
    },
  render: ({state, send}) => {
    let {value, widgets} = state;
    <Embed_Editor
      value
      handleValueChange={value => send(UpdateValue(value))}
      handleEditorUpdate={() => send(EditorUpdate)}
      handleRun={() => send(Run)}
      widgets
    />;
  },
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
