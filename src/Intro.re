open Utils;

module S = {
  let wrapper =
    Css.(
      [%style {|
        flex: 1;
        padding: 0.5rem;
        |}] |. style
    );
};
let component = ReasonReact.statelessComponent("Intro");

let make = _children => {
  ...component,
  render: _self =>
    S.(
      <div className=wrapper>
        <h1> ("Reason REPL" |. str) </h1>
        <p>
          ("Use ; for Reason and ;; OCaml to evaluate the command" |. str)
        </p>
        <p> ("Special commands:" |. str) </p>
        <ul>
          <li>
            <strong> ("#toggle_syntax" |. str) </strong>
            (": switch between OCaml and Reason syntax" |. str)
          </li>
          <li>
            <strong> ("#reset" |. str) </strong>
            (": reset session" |. str)
          </li>
        </ul>
      </div>
    ),
};
