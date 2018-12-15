open Utils;
let component = ReasonReact.statelessComponent("Embed");

let make = _children => {
  ...component,
  render: _self => <p> "Hello world"->str </p>,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
