open Utils;
open Router;
let component = ReasonReact.statelessComponent("Home");

let make = _children => {
  ...component,
  render: _self =>
    <div>
      <h1> ("Coming soon" |. str) </h1>
      <p>
        <Link route=Route.EditorDevelopment>
          ("Editor development" |. str)
        </Link>
      </p>
      <p>
        <Link route=Route.AuthGithub> ("Login with Github" |. str) </Link>
      </p>
      <p> <Link route=Route.NoteNew> ("New note" |. str) </Link> </p>
    </div>,
};
