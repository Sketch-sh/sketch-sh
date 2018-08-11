let component = ReasonReact.statelessComponent("Layout_WithTopbar");

let make = children => {
  ...component,
  render: _self => <> <UI_Topbar /> children </>,
};
