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
  let component = ReasonReact.reducerComponent("AuthStatus.Provider");

  let make = (_children): ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: self => {
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

      self.onUnmount(() =>
        Window.removeEventListener("storage", listener) |> ignore
      );
    },
    render: _self => ReasonReact.null,
  };
};

module IsAuthenticated = {
  let component = ReasonReact.reducerComponent("AuthStatus.IsAuthenticated");
  let make = (children): ReasonReact.component(state, 'a, state) => {
    ...component,
    initialState: () => getCurrentState()->localStorageDataToState,
    reducer: (newStatus, _state) => ReasonReact.Update(newStatus),
    didMount: ({send, onUnmount}) => {
      let id = Store.subscribe(send);
      onUnmount(() => Store.unsubscribe(id));
    },
    render: ({state}) => children(state),
  };
};

module UserInfo = {
  let component = ReasonReact.statelessComponent("AuthStatus.UserInfo");

  [@react.component]
  let make = (~children, ()) => {
    let children = React.Children.toArray(children);
    ReactCompat.useRecordApi({
      ...component,
      render: _self => {
        let%Epitath authState = children =>
          <IsAuthenticated> ...children </IsAuthenticated>;

        switch (authState) {
        | Anonymous => children(None)
        | Login(userId) =>
          open GqlUserInfo;
          let query = UserInfoGql.make(~userId, ());
          let%Epitath {result} = (
            children =>
              <UserInfoComponent variables=query##variables>
                ...children
              </UserInfoComponent>
          );

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
          };
        };
      },
    });
  };
};
