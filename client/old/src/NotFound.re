open Utils;

Modules.require("./NotFound.css");

let component = ReasonReact.statelessComponent("NotFound");

[@react.component]
let make = (~entity="path", ()) =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self =>
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
      </div>,
  });
