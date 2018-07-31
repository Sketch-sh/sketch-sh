let component = ReasonReact.statelessComponent("Index");

let make = _children => {
  ...component,
  render: _self => <Fragment> <Terminal /> <Intro /> </Fragment>,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
