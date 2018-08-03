module Auth = {
  open Dom.Storage;
  let tokenKey = "rtop:token";
  let setToken = token => setItem(tokenKey, token, localStorage);

  let githubLoginRedirect = Config.authDomain ++ "/auth/github";
};
open Utils;

module AuthCallback = {
  let component = ReasonReact.reducerComponent("AuthCallback");

  let make = (~token, _children) : ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: _ => {
      Auth.setToken(token);
      Js.Global.setTimeout(() => ReasonReact.Router.push("/"), 0) |. ignore;
    },
    render: _self => "Authenticating..." |. str,
  };
};

module AuthGithub = {
  let component = ReasonReact.reducerComponent("AuthCallback");

  let make = _children : ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: _ => Router.redirect(Auth.githubLoginRedirect),
    render: _self => "Redirecting to Github..." |. str,
  };
};
