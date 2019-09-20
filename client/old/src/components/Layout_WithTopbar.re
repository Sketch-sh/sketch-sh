let component = ReasonReact.statelessComponent("Layout_WithTopbar");

[@react.component]
let make = (~children, ()) => {
  let children = React.Children.toArray(children);
  ReactCompat.useRecordApi({
    ...component,
    render: _self => <> <UI_Topbar /> children </>,
  });
};
