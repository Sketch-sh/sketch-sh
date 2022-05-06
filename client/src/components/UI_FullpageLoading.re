Modules.import("./UI_FullpageLoading.css");

[@react.component]
let make = () => {
  <div className="UI_FullpageLoading">
    <Logo size=0.3 className="UI_FullpageLoading--logo" />
    <UI_DotFlashing />
  </div>;
};
