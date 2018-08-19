Modules.require("./UI_LoadingPlaceholder.css");

let component = ReasonReact.statelessComponent("UI_LoadingPlaceholder");

let make = children => {
  ...component,
  render: _self =>
    <div className="LoadingPlaceholder">
      <div className="LoadingPlaceholder__animated"> ...children </div>
    </div>,
};

module Rect = {
  let component =
    ReasonReact.statelessComponent("UI_LoadingPlaceholder.Block");

  let make = (~x=?, ~y=?, ~rx=?, ~ry=?, ~height, ~width, _children) => {
    ...component,
    render: _self => {},
  };
};
