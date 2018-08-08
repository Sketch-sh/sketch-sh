module Auth = {
  open Dom.Storage;
  let tokenKey = "rtop:token";
  let setToken = token => setItem(tokenKey, token, localStorage);
  let getToken = () => getItem(tokenKey, localStorage);
  let removeToken = () => removeItem(tokenKey, localStorage);

  let userIdKey = "rtop:userId";
  let setUserId = userId => setItem(userIdKey, userId, localStorage);
  let getUserId = () => getItem(userIdKey, localStorage);
  let removeUserId = () => removeItem(userIdKey, localStorage);

  let githubLoginRedirect = Config.authDomain ++ "/auth/github";

  let decodeUserId = json => Json.Decode.(json |> field("userId", string));

  external toStorageEvent: Dom.event => Webapi.Dom.StorageEvent.t =
    "%identity";
};
open Utils;

module AuthCallback = {
  type state =
    | Initial
    | Success
    | Failure(string);

  type action =
    | ChangeState(state);
  let component = ReasonReact.reducerComponent("AuthCallback");

  let make = (~token, _children) => {
    ...component,
    initialState: () => Initial,
    reducer: (action, _state) =>
      switch (action) {
      | ChangeState(state) =>
        ReasonReact.UpdateWithSideEffects(
          state,
          (
            _ => {
              open Webapi.Dom.Window;
              let window = Webapi.Dom.window;
              switch (window->opener) {
              | Some(_) => window->close
              | None =>
                Js.Global.setTimeout(
                  () => Router.push(Route.Home),
                  switch (state) {
                  | Initial
                  | Success => 1500
                  | Failure(_) => 3000
                  },
                )
                ->ignore
              };
            }
          ),
        )
      },
    didMount: ({send}) =>
      Js.Promise.(
        Jwt.async()
        |> then_(jwt => resolve(jwt->(Jwt.decode(token))))
        |> then_(decoded => {
             Auth.setToken(token);
             Auth.setUserId(Auth.decodeUserId(decoded));
             send(ChangeState(Success));
             resolve();
           })
        |> catch(error => {
             send(ChangeState(Failure("Invalid auth token")));
             resolve(Js.log(error));
           })
      )
      |> ignore,
    render: ({state}) => {
      let message =
        switch (state) {
        | Initial => "Authenticating..."
        | Success => "Authentication successful. Redirecting..."
        | Failure(a) =>
          "Authenticatin failure with the following reason: "
          ++ a
          ++ "\n"
          ++ "Redirecting..."
        };
      message->str;
    },
  };
};

module AuthGithub = {
  let component = ReasonReact.reducerComponent("AuthGithub");

  let make = _children: ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: _ => Router.redirect(Auth.githubLoginRedirect),
    render: _self => "Redirecting to Github..."->str,
  };
};

module AuthLogout = {
  let component = ReasonReact.reducerComponent("AuthGithub");

  let make = _children: ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: _ => {
      open Auth;
      removeToken();
      removeUserId();
      Js.Global.setTimeout(() => Router.push(Route.Home), 0)->ignore;
    },
    render: _self => "Logging out..."->str,
  };
};
