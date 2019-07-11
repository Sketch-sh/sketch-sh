Modules.require("./UI_Topbar.css");
open Utils;

let component = ReasonReact.statelessComponent("UI_Topbar");

let loginButton =
  <Router.Link
    className="btn btn-primary Topbar__login"
    popup=true
    route=Route.AuthGithub>
    <Fi.Github />
    "Login with Github"->str
  </Router.Link>;

let id = "Topbar__actions";

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self =>
      <header className="Topbar">
        <div className="Topbar__left">
          <Router.Link
            route=Route.Home
            className="Topbar__home"
            title="Sketch.sh Homepage">
            <Logo size=0.2 />
            <span> "Sketch.sh"->str </span>
          </Router.Link>
          <div id className="Topbar__actions" />
        </div>
        <div className="Topbar__right">
          <UI_Balloon position=Down message="New sketch">
            ...<Router.Link
                 route={Route.NoteNew(RE)}
                 className="Topbar__action Topbar__action--highlight"
                 title="Create new Sketch">
                 <Fi.Plus />
                 "New Sketch"->str
               </Router.Link>
          </UI_Balloon>
          <AuthStatus.UserInfo>
            ...{user =>
              switch (user) {
              | None => loginButton
              | Some((user, _userId)) => <UI_TopbarUserInfo user />
              }
            }
          </AuthStatus.UserInfo>
        </div>
      </header>,
  });

module Actions = {
  type state = bool;
  type action =
    | Mounted;
  let component = ReasonReact.reducerComponent("Title");
  [@react.component]
  let make = (~children, ()) => {
    let children = React.Children.toArray(children);
    ReactCompat.useRecordApi({
      ...component,
      initialState: () => false,
      reducer: (action, _state) =>
        switch (action) {
        | Mounted => ReasonReact.Update(true)
        },
      didMount: self => self.send(Mounted),
      render: ({state}) =>
        state
          ? switch (Webapi.Dom.(document |> Document.getElementById(id))) {
            | None => ReasonReact.null
            | Some(element) =>
              ReactDOMRe.createPortal(
                children(~buttonClassName="Topbar__action"),
                element,
              )
            }
          : ReasonReact.null,
    });
  };
};
