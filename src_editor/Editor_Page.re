let component = ReasonReact.statelessComponent("Editor_Page");

let make = (~blocks, _children) => {
  ...component,
  render: _self =>
    <div className="pageSizer"> <input /> <Editor_Blocks blocks /> </div>,
};
