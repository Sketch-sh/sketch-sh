open DynamicImport;

[@bs.module "react"]
external lazy_: (unit => Js.Promise.t(DynamicImport.importable('a))) => 'a =
  "lazy";

type nothing;

module type Configuration = {module type t;};

module WithRender = (Config: Configuration) => {
  type childless = array(nothing);
  type renderProp = (module Config.t) => ReasonReact.reactElement;
  type state =
    | Loading
    | Failed(string)
    | Loaded((module Config.t));
  let component = ReasonReact.reducerComponent("Loadable.WithRender");
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
  let make =
      (
        ~fetch,
        ~onFail=_error => ReasonReact.null,
        ~onLoading=() => ReasonReact.null,
        ~delay=200,
        ~render: renderProp,
        _children: childless,
      ) => {
    ...component,
    initialState: () => Loading,
    reducer: (action, _state) =>
      switch (action) {
      | Loading => ReasonReact.Update(Loading)
      | Failed(err) => ReasonReact.Update(Failed(err))
      | Loaded(component) => ReasonReact.Update(Loaded(component))
      },
    didMount: self => {
      let timeoutId =
        Js.Global.setTimeout(
          () =>
            fetch()
            /* Resolve module (unwrap). */
            |> resolve
            /* Resolve new state, user should refine module himself with correct type on render. */
            <$> (data => self.send(Loaded(data)))
            /* Forward error if some trouble happen. */
            <$!> (err => self.send(Failed(err |> Js.String.make)))
            |> ignore,
          delay,
        );
      self.onUnmount(() => Js.Global.clearTimeout(timeoutId));
    },
    render: ({state}) =>
      switch (state) {
      | Loading => onLoading()
      | Failed(err) => onFail(err)
      | Loaded(component) => render(component)
      },
  };
};

module WithChildren = (Config: Configuration) => {
  type renderChild = (module Config.t) => ReasonReact.reactElement;
  type state =
    | Loading
    | Failed(string)
    | Loaded((module Config.t));
  let component = ReasonReact.reducerComponent("Loadable.WithChildren");
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
  let make =
      (
        ~fetch,
        ~onFail=_error => ReasonReact.null,
        ~onLoading=() => ReasonReact.null,
        ~delay=200,
        children: renderChild,
      ) => {
    ...component,
    initialState: () => Loading,
    reducer: (action, _state) =>
      switch (action) {
      | Loading => ReasonReact.Update(Loading)
      | Failed(err) => ReasonReact.Update(Failed(err))
      | Loaded(component) => ReasonReact.Update(Loaded(component))
      },
    didMount: self => {
      let timeoutId =
        Js.Global.setTimeout(
          () =>
            fetch()
            /* Resolve module (unwrap). */
            |> resolve
            /* Resolve new state, user should refine module himself with correct type on render. */
            <$> (data => self.send(Loaded(data)))
            /* Forward error if some trouble happen. */
            <$!> (err => self.send(Failed(err |> Js.String.make)))
            |> ignore,
          delay,
        );
      self.onUnmount(() => Js.Global.clearTimeout(timeoutId));
    },
    render: ({state}) =>
      switch (state) {
      | Loading => onLoading()
      | Failed(err) => onFail(err)
      | Loaded(component) => children(component)
      },
  };
};
