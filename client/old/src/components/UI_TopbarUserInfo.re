Modules.require("./UI_TopbarUserInfo.css");
open Utils;
let component = ReasonReact.statelessComponent("UI_TopbarUserInfo");

[@react.component]
let make = (~user, ()) =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self =>
      <Router.Link
        className="Topbar__userInfo" route={Route.User(user##username)}>
        <div className="Topbar__userInfo--content">
          {user##name
           =>> (
             name =>
               <span className="Topbar__userInfo--name"> name->str </span>
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
      </Router.Link>,
  });
