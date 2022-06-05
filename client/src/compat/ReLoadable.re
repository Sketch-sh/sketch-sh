open DynamicImport;

[@bs.module "react"]
external lazy_: (unit => Js.Promise.t(DynamicImport.importable('a))) => 'a =
  "lazy";

type nothing;

module type Configuration = {module type t;};

module WithRender = (Config: Configuration) => {
  type renderProp = (module Config.t) => ReasonReact.reactElement;
  type state =
    | Loading
    | Failed(string)
    | Loaded((module Config.t));
  /** Our component accept different props :
    *
    * fetch
    * onFail
    * onLoading
    * delay
    * render
    *
    * Children is unused and enforced to be nothing.
    *
    * As you can see, some of props have default value.
  **/
  [@react.component]
  let make =
      (
        ~fetch,
        ~onFail=_error => React.null,
        ~onLoading=() => React.null,
        ~delay=200,
        ~render: renderProp,
      ) => {
    let (state, setState) = React.useState(() => Loading);

    React.useEffect0(() => {
      let timeoutId =
        Js.Global.setTimeout(
          () =>
            fetch()
            /* Resolve module (unwrap). */
            |> resolve
            /* Resolve new state, user should refine module himself with correct type on render. */
            <$> (data => setState(_ => Loaded(data)))
            /* Forward error if some trouble happen. */
            <$!> (err => setState(_ => Failed(err |> Js.String.make)))
            |> ignore,
          delay,
        );

      Some(() => {Js.Global.clearTimeout(timeoutId)});
    });

    switch (state) {
    | Loading => onLoading()
    | Failed(err) => onFail(err)
    | Loaded(component) => render(component)
    };
  };
};

module WithChildren = (Config: Configuration) => {
  type renderChild = (module Config.t) => ReasonReact.reactElement;
  type state =
    | Loading
    | Failed(string)
    | Loaded((module Config.t));
  /** Our component accept different props :
    *
    * fetch
    * onFail
    * onLoading
    * delay
    * render
    *
    * Children is enforced to be a render function.
    *
    * As you can see, some of props have default value.
  **/
  [@react.component]
  let make =
      (
        ~fetch,
        ~onFail=_error => React.null,
        ~onLoading=() => React.null,
        ~delay=200,
        ~children: renderChild,
      ) => {
    let (state, setState) = React.useState(() => Loading);

    React.useEffect0(() => {
      let timeoutId =
        Js.Global.setTimeout(
          () =>
            fetch()
            /* Resolve module (unwrap). */
            |> resolve
            /* Resolve new state, user should refine module himself with correct type on render. */
            <$> (data => setState(_ => Loaded(data)))
            /* Forward error if some trouble happen. */
            <$!> (err => setState(_ => Failed(err |> Js.String.make)))
            |> ignore,
          delay,
        );

      Some(() => {Js.Global.clearTimeout(timeoutId)});
    });

    switch (state) {
    | Loading => onLoading()
    | Failed(err) => onFail(err)
    | Loaded(component) => children(component)
    };
  };
};
