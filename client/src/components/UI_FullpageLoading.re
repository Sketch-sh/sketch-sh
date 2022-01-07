[@bs.config {jsx: 3}];

Modules.import("./UI_FullpageLoading.css");

[@react.component]
let make = () => {
  <div className="UI_FullpageLoading">
    <Logo size=0.3 className="UI_FullpageLoading--logo" />
    <UI_DotFlashing />
  </div>;
};

module Jsx2 = {
  let component = ReasonReact.statelessComponent(__MODULE__);

  let make = children => {
    let children = React.array(children);

    ReasonReactCompat.wrapReactForReasonReact(make, makeProps(), children);
  };
};
