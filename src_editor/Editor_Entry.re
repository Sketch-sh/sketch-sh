let component = ReasonReact.statelessComponent("Editor_Entry");

let make = _children => {...component, render: _self => <Editor_Page />};

let default =
  ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
