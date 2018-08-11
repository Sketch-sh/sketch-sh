/*
   Router:
   This module contains utilities on top of ReasonReact's Router
 */

module Unload = {
  let cb = ref(None);

  let unregister = () => {
    cb := None;
    ();
  };

  let register = newCallback => {
    cb := Some(newCallback);
    unregister;
  };

  [@bs.deriving abstract]
  type window = {
    mutable onbeforeunload:
      (. {. [@bs.set] "returnValue": string}) => Js.Nullable.t(string),
  };

  [@bs.val] external window: window = "";

  module Provider = {
    let component = ReasonReact.reducerComponent("Router_UnloadProvider");

    let make = (_children: React.childless): React.component(unit, 'a, unit) => {
      ...component,
      didMount: _self =>
        window
        ->onbeforeunloadSet(
            (. event) =>
              /* TODO: IE compat: e.returnValue = message; */
              switch (cb^) {
              | None => Js.Nullable.null
              | Some(cb) =>
                switch (cb()) {
                | None => Js.Nullable.null
                | Some(message) =>
                  event##returnValue #= message;
                  Js.Nullable.return(message);
                }
              },
          ),
      render: _self => React.null,
    };
  };
};

let redirect: string => unit = [%bs.raw
  {|
  function (url) {
    window.location.href = url;
  }
|}
];

let pushUnsafe = url => {
  let result =
    switch (Unload.cb^) {
    | None => true
    | Some(cb) =>
      switch (cb()) {
      | None => true
      | Some(message) => Webapi.Dom.(Window.confirm(message, window))
      }
    };
  if (result) {
    ReasonReact.Router.push(url);
  };
};
let push = route => pushUnsafe(Route.routeToUrl(route));

[@bs.send]
external pushState:
  (Dom.history, [@bs.as {json|null|json}] _, [@bs.as ""] _, ~href: string) =>
  unit =
  "";

let pushSilentUnsafe = path =>
  switch ([%external history], [%external window]) {
  | (None, _)
  | (_, None) => ()
  | (Some((history: Dom.history)), Some(_)) =>
    pushState(history, ~href=path)
  };

let pushSilent = route => pushSilentUnsafe(Route.routeToUrl(route));

module LinkUnsafe = {
  let component = ReasonReact.statelessComponent("LinkUnsafe");

  let make = (~href, ~className=?, ~title=?, children) => {
    ...component,
    render: self =>
      <a
        ?className
        ?title
        href
        onClick=(
          self.handle((event, _self) => {
            event->ReactEvent.Mouse.preventDefault;
            pushUnsafe(href);
          })
        )>
        ...children
      </a>,
  };
};

module Link = {
  let component = ReasonReact.statelessComponent("LinkSafe");

  let make = (~route: Route.t, ~className=?, ~title=?, children) => {
    ...component,
    render: _self => {
      let href = Route.routeToUrl(route);
      <LinkUnsafe href ?className ?title> ...children </LinkUnsafe>;
    },
  };
};
