open Utils;
open Router;
let component = ReasonReact.statelessComponent("Home");

let make = _children => {
  ...component,
  render: _self =>
    <div className="Layout__withHeader">
      <UI_Topbar.NoToolbar />
      <p>
        <Link route=Route.EditorDevelopment> "Editor development"->str </Link>
      </p>
      <hr />
      <AuthStatus.IsAuthenticated>
        ...(
             user =>
               switch (user) {
               | Anonymous =>
                 <p>
                   {
                     let href = Route.routeToUrl(Route.AuthGithub);
                     <a
                       href
                       onClick=(
                         event => {
                           event->ReactEvent.Mouse.preventDefault;
                           Popup.openPopup(href);
                         }
                       )>
                       "Login with Github"->str
                     </a>;
                   }
                 </p>
               | Login(userId) =>
                 <div>
                   <UI_UserInfo userId />
                   <p> <Link route=Route.AuthLogout> "Logout"->str </Link> </p>
                 </div>
               }
           )
      </AuthStatus.IsAuthenticated>
      <hr />
      <p> <Link route=Route.NoteNew> "New note"->str </Link> </p>
    </div>,
};
