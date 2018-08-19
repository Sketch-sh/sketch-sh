let component = ReasonReact.statelessComponent("Editor_NotePlaceholder");

let make = _children => {
  ...component,
  render: _self =>
    <div className="EditorNote Layout__center">
      <div style=(ReactDOMRe.Style.make(~marginLeft="30px", ()))>
        <ReactContentLoader
          width=500
          height=100
          style=(ReactDOMRe.Style.make(~width="500px", ~height="100px", ()))>
          <rect x="0" y="0" width="500" height="30" />
          <circle cx="16" cy="50" r="16" />
          <rect x="38" y="42" rx="0" ry="0" width="200" height="16" />
          <rect x="250" y="42" rx="0" ry="0" width="100" height="16" />
        </ReactContentLoader>
      </div>
    </div>,
};
