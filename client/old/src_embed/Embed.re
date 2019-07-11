open Utils;
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

type heightTyp =
  | Fixed
  | Auto;

type workerState = {
  ready: bool,
  packageLoaded: bool,
};

let isReady = s => s.ready && s.packageLoaded;

type state = {
  value: string,
  lang: Editor_Types.lang,
  widgets: array(Editor_Types.Widget.t),
  scrollHeight: ref(float),
  heightTyp,
  loadPackage: option(string),
  workerState,
};

type action =
  | UpdateValue(string)
  | UpdateWidgets(array(Editor_Types.Widget.t))
  | EditorUpdate
  | WorkerPackageLoaded
  | WorkerReady
  | Run;

let component = ReasonReact.reducerComponent("Embed");

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...component,
    didMount: ({state, send, onUnmount}) => {
      Toplevel_Consumer.getWorker() |> ignore;
      Toplevel_Consumer.registerReadyCb(() => send(WorkerReady));
      switch (state.loadPackage) {
      | None => ()
      | Some(package) =>
        let cancelToken =
          Toplevel_Consumer.loadScript(
            package,
            () => {
              Js.log("package loaded");
              send(WorkerPackageLoaded);
            },
          );
        onUnmount(() => Toplevel_Consumer.cancel(cancelToken));
      };
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

      let lang =
        params
        ->URLSearchParams.get("lang")
        ->Belt.Option.getWithDefault("RE");
      let lang =
        switch (lang->Editor_Types.stringToLang) {
        | value => value
        | exception _ => RE
        };
      let heightTyp =
        switch (params->URLSearchParams.get("autoHeight")) {
        | Some("false") => Fixed
        | _ => Auto
        };

      let loadPackage = params->URLSearchParams.get("package");

      {
        value,
        lang,
        widgets: [||],
        scrollHeight: ref(getScrollHeight()),
        heightTyp,
        workerState: {
          ready: false,
          packageLoaded:
            switch (loadPackage) {
            | Some(_) => false
            | None => true
            },
        },
        loadPackage,
      };
    },
    reducer: (action, state) =>
      switch (action) {
      | UpdateValue(value) => ReasonReact.Update({...state, value})
      | UpdateWidgets(widgets) => ReasonReact.Update({...state, widgets})
      | EditorUpdate =>
        ReasonReact.SideEffects(
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
          },
        )
      | Run =>
        ReasonReact.SideEffects(
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
          },
        )
      | WorkerPackageLoaded =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            workerState: {
              ...state.workerState,
              packageLoaded: true,
            },
          },
          ({state, send}) =>
            if (state.workerState->isReady) {
              send(Run);
            },
        )
      | WorkerReady =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            workerState: {
              ...state.workerState,
              ready: true,
            },
          },
          ({state, send}) =>
            if (state.workerState->isReady) {
              send(Run);
            },
        )
      },
    render: ({state, send}) => {
      let {value, widgets, heightTyp} = state;
      <>
        <section
          id="editor"
          className={
            switch (heightTyp) {
            | Fixed => "fixedHeight"
            | Auto => "autoHeight"
            }
          }>
          <Editor_CodeBlock
            value
            firstLineNumber=1
            focused={Some(FcTyp_EditorFocus)}
            widgets
            /* viewportMargin=10. */
            onChange={(value, _) => send(UpdateValue(value))}
            onUpdate=?{
              switch (heightTyp) {
              | Fixed => None
              | Auto => Some(() => send(EditorUpdate))
              }
            }
          />
        </section>
        <footer id="footer">
          <a
            href="https://sketch.sh"
            target="_blank"
            rel="noopener noreferrer"
            className="footer-cell footer-powered-by">
            <span> "Powered by "->str </span>
            <strong> "Sketch"->str </strong>
          </a>
          <span className="footer_spacing" />
          <span className="footer-cell">
            {switch (state.workerState->isReady) {
             | false => "Initialize"->str
             | true => ReasonReact.null
             }}
          </span>
          <button className="footer_run" onClick={_ => send(Run)}>
            <Fi.Play className="footer_run_icon" />
            "Run"->str
          </button>
        </footer>
      </>;
    },
  });

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
