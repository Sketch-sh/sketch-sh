[@bs.config {jsx: 3}];

open Utils;

Modules.require("./NotFound.css");

let make = (~entity="path") => {
  <div className="NotFound">
    <main>
      <section>
        <span> "404"->str </span>
        <p>
          "The requested "->str
          <strong> entity->str </strong>
          " could not be found"->str
        </p>
      </section>
    </main>
  </div>;
};

module Jsx2 = {
  let component = ReasonReact.statelessComponent(__MODULE__);

  let make = children => {
    let children = React.array(children);

    ReasonReactCompat.wrapReactForReasonReact(make, makeProps(), children);
  };
};
