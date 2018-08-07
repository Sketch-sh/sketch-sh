/*
   Router:
   This module contains utilities on top of ReasonReact's Router
 */

let redirect: string => unit = [%bs.raw
  {|
  function (url) {
    window.location.href = url;
  }
|}
];

let pushUnsafe = url => ReasonReact.Router.push(url);
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
            ReasonReact.Router.push(href);
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
