let component = ReasonReact.statelessComponent("Index");

let make = _children => {
  ...component,
  render: _self => <Page message="Hello!" />,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
