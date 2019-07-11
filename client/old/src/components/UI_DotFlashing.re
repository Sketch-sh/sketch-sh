Modules.require("./UI_DotFlashing.css");

let component = ReasonReact.statelessComponent("UI_DotFlashing");

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self => <div className="dot-flashing" />,
  });
