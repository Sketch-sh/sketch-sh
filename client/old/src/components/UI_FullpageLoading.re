Modules.import("./UI_FullpageLoading.css");

let component = ReasonReact.statelessComponent("UI_FullpageLoading");

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self =>
      <div className="UI_FullpageLoading">
        <Logo size=0.3 className="UI_FullpageLoading--logo" />
        <UI_DotFlashing />
      </div>,
  });
