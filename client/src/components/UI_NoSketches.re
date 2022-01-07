[@bs.config {jsx: 3}];
open Utils;

Modules.require("./UI_NoSketches.css");

[@react.component]
let make = (~className=?) => {
  <section ?className>
    <Fi.Package className="UI_NoSketches--icon" />
    "Your sketches will show up here"->str
    <Router.Link
      route=Route.NoteTemplateChoose
      className="btn btn-primary btn--large UI_NoSketches--button"
      role="button">
      <Fi.Plus />
      "Create New Sketch"->str
    </Router.Link>
  </section>;
};

module Jsx2 = {
  let component = ReasonReact.statelessComponent(__MODULE__);
  let make = (~className=?) => {
    ReasonReactCompat.wrapReactForReasonReact(
      make,
      makeProps(~className?, ()),
    );
  };
};
