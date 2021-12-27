[@bs.config {jsx: 3}];

Modules.require("./UI_DotFlashing.css");

[@react.component]
let make = () => <div className="dot-flashing" />;

module Jsx2 = {
  let component = ReasonReact.statelessComponent(__MODULE__);

  let make = children => {
    ReasonReactCompat.wrapReactForReasonReact(make, makeProps(), children);
  };
};
