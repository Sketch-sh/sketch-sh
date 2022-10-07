module type LocalStorageKey = {let key: string;};
module LocalStorageOperaton = (K: LocalStorageKey) => {
  open Dom.Storage;
  let key = K.key;
  let set = value => setItem(key, value, localStorage);
  let get = () => getItem(key, localStorage);
  let remove = () => removeItem(key, localStorage);
};

module Auth = {
  module Token =
    LocalStorageOperaton({
      let key = "rtop:token";
    });

  module UserId =
    LocalStorageOperaton({
      let key = "rtop:userId";
    });

  module EditToken =
    LocalStorageOperaton({
      let key = "rtop:editToken";
    });

  let getOrCreateEditToken = () =>
    switch (EditToken.get()) {
    | None =>
      let newToken = Utils.generateId();
      EditToken.set(newToken);
      newToken;
    | Some(editToken) => editToken
    };

  let githubLoginRedirect = "/api/auth/github";

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

  [@react.component]
  let make = (~token) =>
    ReactCompat.useRecordApi({
      ...ReactCompat.component,
      initialState: () => Initial,
      reducer: (action, _state) =>
        switch (action) {
        | ChangeState(state) =>
          UpdateWithSideEffects(
            state,
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
            },
          )
        },
      didMount: ({send}) =>
        Js.Promise.(
          Jwt.async()
          |> then_(jwt => resolve(jwt->(Jwt.decode(token))))
          |> then_(decoded => {
               Auth.Token.set(token);
               Auth.UserId.set(Auth.decodeUserId(decoded));
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
    });
};

module AuthGithub = {
  [@react.component]
  let make = () => {
    React.useEffect0(() => {
      Router.redirect(Auth.githubLoginRedirect);
      None;
    });
    "Redirecting to Github..."->str;
  };
};
