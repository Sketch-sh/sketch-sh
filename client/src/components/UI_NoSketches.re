Modules.require("./UI_NoSketches.css");
open Utils;

let component = ReasonReact.statelessComponent("UI_NoSketches");

let make = (~className=?, _children) => {
  ...component,
  render: _self =>
    <section className=(Cn.make(["UI_NoSketches", Cn.unwrap(className)]))>
      <Fi.Package className="UI_NoSketches--icon" />
      "Your sketches will show up here"->str
      <Router.Link
        route=Route.NoteNew
        className="btn btn-primary btn--large UI_NoSketches--button"
        role="button">
        <Fi.Plus />
        "Create New Sketch"->str
      </Router.Link>
    </section>,
};
