let component = ReasonReact.statelessComponent("Editor_Page");

let make = (~blocks, _children) => {
  ...component,
  render: _self =>
    <div className="pageSizer">
      <div className="metadata">
        <input className="metadata-input" placeholder="untitled note" />
      </div>
      <Editor_Blocks blocks />
    </div>,
};
