open Utils;
Modules.require("../src/App.css");
Modules.require("./Embed.css");

[@bs.val] external atob: string => string = "atob";
[@bs.val] external btoa: string => string = "btoa";

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
  scrollHeight: float,
  heightTyp,
  loadPackage: option(string),
  workerState,
  isRun: bool,
};

type action =
  | UpdateValue(string)
  | UpdateWidgets(array(Editor_Types.Widget.t))
  | EditorUpdate
  | WorkerPackageLoaded
  | WorkerReady
  | Run;

let reducer = (state, action) =>
  switch (action) {
  | UpdateValue(value) => {...state, value}
  | UpdateWidgets(widgets) => {...state, widgets}
  | EditorUpdate =>
    let scrollHeight = {
      let newScrollHeight = getScrollHeight();

      if (newScrollHeight != state.scrollHeight) {
        let src: string = [%bs.raw "window.location.href"];
        postMessage({
          "type": "iframe.resize",
          "src": src,
          "height": newScrollHeight,
        });
        newScrollHeight;
      } else {
        state.scrollHeight;
      };
    };
    {...state, scrollHeight};
  | Run => {...state, isRun: true}
  | WorkerPackageLoaded => {
      ...state,
      workerState: {
        ...state.workerState,
        packageLoaded: true,
      },
    }
  | WorkerReady => {
      ...state,
      workerState: {
        ...state.workerState,
        ready: true,
      },
    }
  };

[@react.component]
let make = () => {
  let (state, dispatch) = {
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

    let heightTyp =
      switch (params->URLSearchParams.get("autoHeight")) {
      | Some("false") => Fixed
      | _ => Auto
      };

    let loadPackage = params->URLSearchParams.get("package");

    React.useReducer(
      reducer,
      {
        value,
        lang,
        widgets: [||],
        scrollHeight: getScrollHeight(),
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
        isRun: false,
      },
    );
  };

  let {value, widgets, heightTyp, loadPackage, workerState, isRun} = state;

  React.useEffect0(() => {
    Toplevel_Consumer.getWorker() |> ignore;
    Toplevel_Consumer.registerReadyCb(() => {
      dispatch @@ WorkerReady;
      if (workerState->isReady) {
        dispatch @@ Run;
      };
    });

    Some(
      () => {
        switch (loadPackage) {
        | None => ()
        | Some(package) =>
          let cancelToken =
            Toplevel_Consumer.loadScript(
              package,
              () => {
                Js.log("package loaded");
                dispatch @@ WorkerPackageLoaded;
                if (workerState->isReady) {
                  dispatch @@ Run;
                };
              },
            );
          Toplevel_Consumer.cancel(cancelToken);
        };

        if (isRun) {
          let cancelToken =
            Toplevel_Consumer.executeEmbed(
              ~lang=state.lang,
              ~code=state.value,
              fun
              | Belt.Result.Ok(result) => {
                  let widgets =
                    Editor_Blocks_Utils.executeResultToWidget(result);
                  dispatch @@ UpdateWidgets(widgets);
                }
              | Belt.Result.Error(message) => Js.log(message),
            );
          Toplevel_Consumer.cancel(cancelToken);
        };
      },
    );
  });

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
        onChange={(value, _) => dispatch @@ UpdateValue(value)}
        onUpdate=?{
          switch (heightTyp) {
          | Fixed => None
          | Auto => Some(() => dispatch @@ EditorUpdate)
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
        {switch (workerState->isReady) {
         | false => "Initialize"->str
         | true => React.null
         }}
      </span>
      <button className="footer_run" onClick={_ => dispatch @@ Run}>
        <Fi.Play className="footer_run_icon" />
        "Run"->str
      </button>
    </footer>
  </>;
};