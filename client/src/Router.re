/*
   Router:
   This module contains utilities on top of ReasonReact's Router
 */

module Unload = {
  type message = ref(option(string));
  let message: message = ref(None);

  type callback = option(message => unit);
  let cb: ref(callback) = ref(None);

  let register = (callback: message => unit) => {
    cb := Some(callback);
    ();
  };

  let unregister = () => {
    cb := None;
    message := None;
    ();
  };

  let getUnloadMessage: unit => option(string) =
    () =>
      switch (cb^) {
      | None => None
      | Some(cb) =>
        cb(message);
        message^;
      };
  [@bs.deriving abstract]
  type window = {
    mutable onbeforeunload:
      (. {. [@bs.set] "returnValue": string}) => Js.Nullable.t(string),
  };

  [@bs.val] external window: window = "window";

  module Provider = {
    [@react.component]
    let make = _children => {
      React.useEffect0(() => {
        window->onbeforeunloadSet((. event) =>
          switch (getUnloadMessage()) {
          | None => Js.Nullable.null
          | Some(message) =>
            event##returnValue #= message;
            Js.Nullable.return(message);
          }
        );
        None;
      });
      ReactOld.null;
    };
  };
};

let redirect: string => unit =
  url => Webapi.Dom.(location->Location.setHref(url));

let pushUnsafe = url => {
  let allowRouteTransition =
    switch (Unload.getUnloadMessage()) {
    | None => true
    | Some(message) => Webapi.Dom.(Window.confirm(message, window))
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
  "pushState";

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
  "replaceState";

let replaceSilentUnsafe = path =>
  switch ([%external history], [%external window]) {
  | (None, _)
  | (_, None) => ()
  | (Some((history: Dom.history)), Some(_)) =>
    replaceState(history, ~href=path)
  };

let replaceSilent = route => replaceSilentUnsafe(Route.routeToUrl(route));

module LinkUnsafe = {
  [@react.component]
  let make =
      (~href, ~id=?, ~className=?, ~title=?, ~popup, ~role=?, ~children) => {
    <a
      ?id
      ?className
      ?title
      ?role
      href
      onClick={event =>
        if (!event->ReactEvent.Mouse.ctrlKey
            && event->ReactEvent.Mouse.button != 1) {
          event->ReactEvent.Mouse.preventDefault;
          if (popup) {
            Popup.openPopup(href);
          } else {
            pushUnsafe(href);
          };
        }
      }>
      children
    </a>;
  };
};

module Link = {
  [@react.component]
  let make =
      (
        ~route: Route.t,
        ~id=?,
        ~title=?,
        ~className=?,
        ~popup=false,
        ~role=?,
        ~children,
      ) => {
    let href = Route.routeToUrl(route);
    <LinkUnsafe href ?id ?className ?title popup ?role> children </LinkUnsafe>;
  };
};
