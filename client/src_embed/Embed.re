Modules.require("../src/App.css");
Modules.require("./Embed.css");

let component = ReasonReact.statelessComponent("Embed");

let make = _children => {
  ...component,
  render: _self => <Embed_Editor initialValue="1+1;" />,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
