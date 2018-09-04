/*
   Router:
   This module contains utilities on top of ReasonReact's Router
 */

module Unload = {
  type message = ref(option(string));
  let message: message = ref(None);

  type callback = option(message => unit);
  let cb: ref(callback) = ref(None);

  let unregister = () => {
    cb := None;
    message := None;
    ();
  };

  let setMessage = content => message := content;
  let register = (callback: message => unit) => {
    cb := Some(callback);
    ();
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
              switch (cb^) {
              | None => Js.Nullable.null
              | Some(cb) =>
                cb(message);
                switch (message^) {
                | None => Js.Nullable.null
                | Some(message) =>
                  event##returnValue #= message;
                  Js.Nullable.return(message);
                };
              },
          ),
      render: _self => React.null,
    };
  };
};

let redirect: string => unit =
  url => Webapi.Dom.(location->Location.setHref(url));

let pushUnsafe = url => {
  let allowRouteTransition =
    switch (Unload.cb^) {
    | None => true
    | Some(cb) =>
      cb(Unload.message);
      switch (Unload.message^) {
      | None => true
      | Some(message) => Webapi.Dom.(Window.confirm(message, window))
      };
    };
  if (allowRouteTransition) {
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

[@bs.send]
external replaceState:
  (Dom.history, [@bs.as {json|null|json}] _, [@bs.as ""] _, ~href: string) =>
  unit =
  "";

let replaceSilentUnsafe = path =>
  switch ([%external history], [%external window]) {
  | (None, _)
  | (_, None) => ()
  | (Some((history: Dom.history)), Some(_)) =>
    replaceState(history, ~href=path)
  };

let replaceSilent = route => replaceSilentUnsafe(Route.routeToUrl(route));

module LinkUnsafe = {
  let component = ReasonReact.statelessComponent("LinkUnsafe");

  let make = (~href, ~id=?, ~className=?, ~title=?, ~popup, ~role=?, children) => {
    ...component,
    render: self =>
      <a
        ?id
        ?className
        ?title
        ?role
        href
        onClick=(
          self.handle((event, _self) =>
            if (!event->ReactEvent.Mouse.ctrlKey
                && event->ReactEvent.Mouse.button != 1) {
              event->ReactEvent.Mouse.preventDefault;
              if (popup) {
                Popup.openPopup(href);
              } else {
                pushUnsafe(href);
              };
            }
          )
        )>
        ...children
      </a>,
  };
};

module Link = {
  let component = ReasonReact.statelessComponent("LinkSafe");

  let make =
      (
        ~route: Route.t,
        ~id=?,
        ~title=?,
        ~className=?,
        ~popup=false,
        ~role=?,
        children,
      ) => {
    ...component,
    render: _self => {
      let href = Route.routeToUrl(route);
      <LinkUnsafe href ?id ?className ?title popup ?role>
        ...children
      </LinkUnsafe>;
    },
  };
};
