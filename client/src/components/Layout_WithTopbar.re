/*
 [@bs.config {jsx: 3}];

 [@react.component]
 let make = (~children) => {<> <UI_Topbar /> children </>};

 module Jsx2 = {
   let component = ReasonReact.statelessComponent(__MODULE__);
   let make = (children) => {
     ReasonReactCompat.wrapReactForReasonReact(
       make,
       makeProps(~children, ()),
     );
   };
 };
 */
let component = ReasonReact.statelessComponent("Layout_WithTopbar");

let make = children => {
  ...component,
  render: _self => <> <UI_Topbar /> children </>,
};
