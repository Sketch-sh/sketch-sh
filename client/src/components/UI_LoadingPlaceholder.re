Modules.require("./UI_LoadingPlaceholder.css");

let component = ReasonReact.statelessComponent("UI_LoadingPlaceholder");

let make = (~className=?, children) => {
  ...component,
  render: _self =>
    <div className=(Cn.make(["LoadingPlaceholder", Cn.unwrap(className)]))>
      <div className="LoadingPlaceholder__animated"> ...children </div>
    </div>,
};

module Rect = {
  let component =
    ReasonReact.statelessComponent("UI_LoadingPlaceholder.Block");

  let make =
      (~top=?, ~left=?, ~right=?, ~bottom=?, ~height=?, ~width=?, _children) => {
    ...component,
    render: _self =>
      <div
        className="LoadingPlaceholder__masker"
        style=(
          ReactDOMRe.Style.make(
            ~top?,
            ~left?,
            ~right?,
            ~bottom?,
            ~height?,
            ~width?,
            (),
          )
        )
      />,
  };
};
