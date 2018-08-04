open Utils;
let component = ReasonReact.statelessComponent("NotFound");

let make = (~entity="path", _children) => {
  ...component,
  render: _self =>
    <main>
      <section>
        <span> ("404" |. str) </span>
        <p> ("The requested " ++ entity ++ " could not be found" |. str) </p>
      </section>
    </main>,
};
