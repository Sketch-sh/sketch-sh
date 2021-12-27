let component = ReasonReact.statelessComponent("UI_FullpageLoading");
Modules.import("./UI_FullpageLoading.css");

let make = _children => {
  ...component,
  render: _self =>
    <div className="UI_FullpageLoading">
      <Logo.Jsx2 size=0.3 className="UI_FullpageLoading--logo" />
      <UI_DotFlashing.Jsx2 />
    </div>,
};
