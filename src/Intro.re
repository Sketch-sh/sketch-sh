open Utils;

module S = {
  let wrapper = Css.([%style {|
        flex: 1;
        |}] |. style);
};
let component = ReasonReact.statelessComponent("Intro");

let make = _children => {
  ...component,
  render: _self =>
    S.(<div className=wrapper> <h1> ("Reason REPL" |. str) </h1> </div>),
};
