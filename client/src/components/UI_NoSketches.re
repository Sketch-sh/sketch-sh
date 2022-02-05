Modules.import("./UI_NoSketches.css");
open Utils;

[@react.component]
let make = (~className=?) => {
  <section className={Cn.make(["UI_NoSketches", Cn.unwrap(className)])}>
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
