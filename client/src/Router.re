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
external pushState :
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

[@bs.deriving abstract]
type linkProps = {
  [@bs.optional]
  className: string,
  [@bs.optional]
  title: string,
  href: string,
  onClick: ReactEventRe.Mouse.t => unit,
};

external hack : linkProps => Js.t({..}) = "%identity";

module LinkUnsafe = {
  let component = ReasonReact.statelessComponent("LinkUnsafe");

  let make = (~href, ~className=?, ~title=?, children) => {
    ...component,
    render: self =>
      ReasonReact.createDomElement(
        "a",
        ~props=
          linkProps(
            ~className?,
            ~title?,
            ~href,
            ~onClick=
              self.handle((event, _self) => {
                ReactEventRe.Mouse.preventDefault(event);
                ReasonReact.Router.push(href);
              }),
            (),
          )
          |. hack,
        children,
      ),
  };
};

module Link = {
  let component = ReasonReact.statelessComponent("LinkSafe");

  let make = (~route: Route.t, ~className=?, ~title=?, children) => {
    ...component,
    render: self => {
      let href = Route.routeToUrl(route);
      ReasonReact.createDomElement(
        "a",
        ~props=
          linkProps(
            ~className?,
            ~title?,
            ~href,
            ~onClick=
              self.handle((event, _self) => {
                ReactEventRe.Mouse.preventDefault(event);
                ReasonReact.Router.push(href);
              }),
            (),
          )
          |. hack,
        children,
      );
    },
  };
};
