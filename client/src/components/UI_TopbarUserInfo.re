Modules.require("./UI_TopbarUserInfo.css");
open Utils;
let component = ReasonReact.statelessComponent("UI_TopbarUserInfo");

let make = (~user, _children) => {
  ...component,
  render: _self =>
    <Router.Link
      className="Topbar__userInfo"
      route=(Route.User(optionToEmptyString(user##username)))>
      <div className="Topbar__userInfo--content">
        (
          user##name
          =>> (
            name => <span className="Topbar__userInfo--name"> name->str </span>
          )
        )
        (
          user##username
          =>> (
            username =>
              <span className="Topbar__userInfo--username">
                username->str
              </span>
          )
        )
      </div>
      (
        user##avatar
        =>> (
          avatar =>
            <img
              src=avatar
              width="40"
              height="40"
              className="Topbar__userInfo--avatar"
              alt=(optionToEmptyString(user##username) ++ " avatar")
            />
        )
      )
    </Router.Link>,
};
