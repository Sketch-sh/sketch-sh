let component = ReasonReact.statelessComponent("Editor_NotePlaceholder");

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self =>
      <div className="Layout__center">
        <div
          style={ReactDOMRe.Style.make(
            ~marginLeft="30px",
            ~maxWidth="800px",
            (),
          )}>
          <div>
            <ReactContentLoader width=600 height=100>
              <rect rx="4" ry="4" width="600" height="30" />
              <circle cx="16" cy="50" r="16" />
              <rect x="38" y="42" rx="4" ry="4" width="300" height="16" />
              <rect x="350" y="42" rx="4" ry="4" width="200" height="16" />
            </ReactContentLoader>
          </div>
          <div>
            <ReactContentLoader width=400 height=100>
              <rect x="0" y="0" rx="3" ry="3" width="70" height="10" />
              <rect x="80" y="0" rx="3" ry="3" width="100" height="10" />
              <rect x="190" y="0" rx="3" ry="3" width="20" height="10" />
              <rect x="220" y="0" rx="3" ry="3" width="80" height="10" />
              /* Row */
              <rect x="15" y="20" rx="3" ry="3" width="130" height="10" />
              <rect x="155" y="20" rx="3" ry="3" width="130" height="10" />
              /* Row */
              <rect x="15" y="40" rx="3" ry="3" width="50" height="10" />
              <rect x="75" y="40" rx="3" ry="3" width="130" height="10" />
              <rect x="215" y="40" rx="3" ry="3" width="100" height="10" />
              /* Row */
              <rect x="15" y="60" rx="3" ry="3" width="90" height="10" />
              <rect x="115" y="60" rx="3" ry="3" width="60" height="10" />
              <rect x="185" y="60" rx="3" ry="3" width="60" height="10" />
              /* Row */
              <rect x="0" y="80" rx="3" ry="3" width="30" height="10" />
            </ReactContentLoader>
          </div>
          <div> <ReactContentLoader width=400 height=90 typ=Code /> </div>
        </div>
      </div>,
  });
