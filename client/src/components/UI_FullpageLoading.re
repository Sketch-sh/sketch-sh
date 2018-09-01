Modules.import("./UI_FullpageLoading.css");

let component = ReasonReact.statelessComponent("UI_FullpageLoading");

let make = _children => {
  ...component,
  render: _self =>
    <div className="UI_FullpageLoading">
      <Logo size=0.3 className="UI_FullpageLoading--logo" />
      <UI_DotFlashing />
    </div>,
};
