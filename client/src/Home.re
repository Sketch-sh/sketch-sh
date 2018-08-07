open Utils;
open Router;
let component = ReasonReact.statelessComponent("Home");

let make = _children => {
  ...component,
  render: _self =>
    <div>
      <h1> "Coming soon"->str </h1>
      <p>
        <Link route=Route.EditorDevelopment> "Editor development"->str </Link>
      </p>
      <hr />
      <Auth.IsAuthenticated>
        ...(
             state =>
               switch (state) {
               | None =>
                 <p>
                   <Link route=Route.AuthGithub>
                     "Login with Github"->str
                   </Link>
                 </p>
               | Some(userId) =>
                 <div>
                   <UI_UserInfo userId />
                   <p> <Link route=Route.AuthLogout> "Logout"->str </Link> </p>
                 </div>
               }
           )
      </Auth.IsAuthenticated>
      <hr />
      <p> <Link route=Route.NoteNew> "New note"->str </Link> </p>
    </div>,
};
