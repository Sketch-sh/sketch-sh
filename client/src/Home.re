open Utils;
open Router;
let component = ReasonReact.statelessComponent("Home");

let make = _children => {
  ...component,
  render: _self =>
    <div>
      <h1> ("Coming soon" |. str) </h1>
      <p>
        <Link href="/thangngoc89/somerandomnote">
          ("Editor development" |. str)
        </Link>
      </p>
      <p> <Link href="/auth/github"> ("Login with Github" |. str) </Link> </p>
      <p> <Link href="/new"> ("New note" |. str) </Link> </p>
    </div>,
};
