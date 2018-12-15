Modules.require("../src/App.css");
Modules.require("./Embed.css");

[@bs.val] external atob: string => string = "";
[@bs.val] external btoa: string => string = "";

type handler('a) = Js.t('a) => unit;
[@bs.val] external window: Dom.window = "";
[@bs.send]
external addEventListener: (Dom.window, string, handler('a)) => unit = "";

type state = {
  value: string,
  widgets: array(Editor_Types.Widget.t),
};

type action =
  | UpdateValue(string)
  | UpdateWidgets(array(Editor_Types.Widget.t))
  | Run;

let component = ReasonReact.reducerComponent("Embed");

let make = _children => {
  ...component,
  didMount: ({state: _, send}) => {
    let resizeHandler = event => Js.log(event);

    window->addEventListener("resize", resizeHandler);

    send(Run);
    ();
  },
  initialState: () => {
    let search = [%bs.raw "window.location.search"];
    let params = URLSearchParams.make(search);
    let value = params->URLSearchParams.get("value");
    let value =
      switch (value) {
      | None => ""
      | Some(value) => atob(value)
      };
    {value, widgets: [||]};
  },
  reducer: (action, state) =>
    switch (action) {
    | UpdateValue(value) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, value},
        (_self => Webapi.Dom.(Js.log(window->Window.innerHeight))),
      )
    | UpdateWidgets(widgets) => ReasonReact.Update({...state, widgets})
    | Run =>
      ReasonReact.SideEffects(
        (
          ({state, send, onUnmount}) => {
            let cancelToken =
              Toplevel_Consumer.executeEmbed(
                ~lang=RE,
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
      handleRun={() => send(Run)}
      widgets
    />;
  },
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
