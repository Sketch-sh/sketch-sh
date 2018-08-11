Modules.require("./UI_DotFlashing.css");

let component = ReasonReact.statelessComponent("UI_DotFlashing");

let make = _children => {
  ...component,
  render: _self => <div className="dot-flashing" />,
};
