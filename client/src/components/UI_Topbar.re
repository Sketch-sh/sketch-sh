Modules.require("./UI_Topbar.css");
open Utils;

module WithToolbar = {
  let component = ReasonReact.statelessComponent("UI_Topbar");

  let loginButton = {
    let href = Route.routeToUrl(Route.AuthGithub);
    <a
      href
      className="Topbar__login"
      onClick=(
        event => {
          event->ReactEvent.Mouse.preventDefault;
          Popup.openPopup(href);
        }
      )>
      <Fi.Github />
      "Login with Github"->str
    </a>;
  };

  let make = children => {
    ...component,
    render: _self =>
      <header className="Topbar">
        <div className="Topbar__left">
          <Router.Link route=Route.Home className="Topbar__home">
            "ReasonML playground"->str
          </Router.Link>
          <div id="Topbar__actions" className="Topbar__actions">
            (children(~buttonClassName="Topbar__action"))
          </div>
        </div>
        <div className="Topbar__right">
          <AuthStatus.UserInfo>
            ...(
                 user =>
                   switch (user) {
                   | None => loginButton
                   | Some((user, _userId)) => <UI_TopbarUserInfo user />
                   }
               )
          </AuthStatus.UserInfo>
        </div>
      </header>,
  };
};

module NoToolbar = {
  let component = ReasonReact.statelessComponent("UI_TopbarNoToolbar");

  let make = _children => {
    ...component,
    render: _self =>
      <WithToolbar>
        ...((~buttonClassName as _) => ReasonReact.null)
      </WithToolbar>,
  };
};
