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

let push = url => ReasonReact.Router.push(url);

module Link = {
  let component = ReasonReact.statelessComponent("Link");

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
