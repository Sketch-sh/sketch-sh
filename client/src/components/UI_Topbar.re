Modules.require("./UI_Topbar.css");
open Utils;

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

let id = "Topbar__actions";

let make = _children => {
  ...component,
  render: _self =>
    <header className="Topbar">
      <div className="Topbar__left">
        <Router.Link route=Route.Home className="Topbar__home">
          "Sketch.sh"->str
        </Router.Link>
        <div id className="Topbar__actions" />
      </div>
      <div className="Topbar__right">
        <UI_Balloon position=Down message="New sketch">
          ...<Router.Link
               route=Route.NoteNew
               className="Topbar__action Topbar__action--highlight">
               <Fi.Plus />
               "New Sketch"->str
             </Router.Link>
        </UI_Balloon>
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

module Actions = {
  type state = bool;
  type action =
    | Mounted;
  let component = ReasonReact.reducerComponent("Title");
  let make = children => {
    ...component,
    initialState: () => false,
    reducer: (action, _state) =>
      switch (action) {
      | Mounted => ReasonReact.Update(true)
      },
    didMount: self => self.send(Mounted),
    render: ({state}) =>
      state ?
        switch (Webapi.Dom.(document |> Document.getElementById(id))) {
        | None => ReasonReact.null
        | Some(element) =>
          ReactDOMRe.createPortal(
            children(~buttonClassName="Topbar__action"),
            element,
          )
        } :
        ReasonReact.null,
  };
};
