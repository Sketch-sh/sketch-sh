open Utils;
module Auth = Auth.Auth;
let getCurrentState = Auth.UserId.get;

type state =
  | Anonymous
  | Login(string);

let authStateToUserId =
  fun
  | Anonymous => Config.anonymousUserId
  | Login(id) => id;

let localStorageDataToState =
  fun
  | None => Anonymous
  | Some(id) => Login(id);

module Store = {
  module MI = Belt.Map.Int;
  let subscribers = ref(MI.empty);
  let nextId = ref(0);
  let subscribe = subscriber => {
    nextId := nextId^ + 1;
    subscribers := (subscribers^)->(MI.set(nextId^, subscriber));
    nextId^;
  };
  let unsubscribe = id => subscribers := (subscribers^)->MI.remove(id);

  let broadcast = newValue =>
    MI.forEach(subscribers^, (_key, subscriber) => subscriber(newValue));
};

module Provider = {
  let make = () => {
    React.useEffect0(() => {
      let _ =
        Auth.(
          /*
           * This set edit token on initial load
           * only if user is not login
           * AND token is empty
           */
          switch (UserId.get()) {
          | Some(_) => ()
          | None =>
            switch (EditToken.get()) {
            | None => EditToken.set(Utils.generateId())
            | Some(_) => ()
            }
          }
        );
      open Webapi.Dom;

      let listener = event => {
        let event = Auth.toStorageEvent(event);
        let key = event->StorageEvent.key;
        if (key == Auth.UserId.key) {
          let newValue =
            localStorageDataToState(
              event
              ->StorageEvent.newValue
              ->Utils.toNullable
              ->Js.Nullable.toOption,
            );

          Store.broadcast(newValue);
        };
      };
      window |> Window.addEventListener("storage", listener);

      Some(() => Window.removeEventListener("storage", listener) |> ignore);
    });

    ReasonReact.null;
  };
};

module IsAuthenticated = {
  [@react.component]
  let make = (~children) => {
    let (state, setState) =
      React.useState(_ => getCurrentState()->localStorageDataToState);

    React.useEffect0(() => {
      let id = Store.subscribe(newState => setState(_ => newState));

      Some(() => {Store.unsubscribe(id)});
    });

    children(state);
  };
};

module UserInfo = {
  [@react.component]
  let make = (~children) => {
    <IsAuthenticated>
      {authState =>
         switch (authState) {
         | Anonymous => children(None)
         | Login(userId) =>
           open GqlUserInfo;
           let query = UserInfoGql.make(~userId, ());
           <UserInfoComponent variables=query##variables>
             ...{({result}) =>
               switch (result) {
               | Loading => children(None)
               | Error(_) => children(None)
               | Data(response) =>
                 response##user
                 ->(
                     arrayFirst(~empty=children(None), ~render=user =>
                       children(Some((user, userId)))
                     )
                   )
               }
             }
           </UserInfoComponent>;
         }}
    </IsAuthenticated>;
  };
};
