let component = ReasonReact.statelessComponent("UI_LoadingPlaceholder");

let make = _children => {
  ...component,
  render: _self =>
    <div className="LoadingPlaceholder">
      <div className="LoadingPlaceholder__animated" />
    </div>,
};
