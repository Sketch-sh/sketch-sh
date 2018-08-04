module Auth = {
  open Dom.Storage;
  let tokenKey = "rtop:token";
  let setToken = token => setItem(tokenKey, token, localStorage);

  let userIdKey = "rtop:userId";
  let setUserId = userId => setItem(userIdKey, userId, localStorage);

  let githubLoginRedirect = Config.authDomain ++ "/auth/github";

  let getUserId = json => Json.Decode.(json |> field("userId", string));
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
            _ =>
              Js.Global.setTimeout(
                () => Router.push(Route.Home),
                switch (state) {
                | Initial
                | Success => 1500
                | Failure(_) => 3000
                },
              )
              |. ignore
          ),
        )
      },
    didMount: ({send}) =>
      Js.Promise.(
        Jwt.async()
        |> then_(jwt => resolve(jwt |. Jwt.decode(token)))
        |> then_(decoded => {
             switch (decoded |. Js.Nullable.toOption) {
             | None => send(ChangeState(Failure("Invalid auth token")))
             | Some(decoded) =>
               Auth.setToken(token);
               Auth.setToken(Auth.getUserId(decoded));

               send(ChangeState(Success));
             };
             resolve();
           })
        |> catch(error => {
             send(ChangeState(Failure("Unknown")));
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
      message |. str;
    },
  };
};

module AuthGithub = {
  let component = ReasonReact.reducerComponent("AuthGithub");

  let make = _children : ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: _ => Router.redirect(Auth.githubLoginRedirect),
    render: _self => "Redirecting to Github..." |. str,
  };
};
