Modules.require("./UI_TopbarUserInfo.css");

open Utils;
let component = ReasonReact.statelessComponent("UI_TopbarUserInfo");

let make = (~user, _children) => {
  ...component,
  render: _self =>
    <div className="Topbar__userInfo Topbar__userInfo--dropdown">
      <div className="Topbar__userInfo--content">
        {user##name
         =>> (
           name => <span className="Topbar__userInfo--name"> name->str </span>
         )}
        <span className="Topbar__userInfo--username">
          {user##username->str}
        </span>
      </div>
      {user##avatar
       =>> (
         avatar =>
           <img
             src=avatar
             width="40"
             height="40"
             className="Topbar__userInfo--avatar"
             alt={user##username ++ " avatar"}
           />
       )}
      <div className="Topbar__userInfo--dropdownMenu">
        <div className="Topbar__userInfo--dropdownMenuInner">
          <Router.Link
            className="Topbar__userInfo--dropdownItem"
            route={Route.User(user##username)}>
            "Your sketches"->str
          </Router.Link>
          <Router.Link
            className="Topbar__userInfo--dropdownItem" route=Route.AuthLogout>
            "Logout"->str
          </Router.Link>
        </div>
      </div>
    </div>,
};
