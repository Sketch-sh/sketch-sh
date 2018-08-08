open Utils;
module Auth = Auth.Auth;
let getCurrentState = Auth.getUserId;

type state = option(int);

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

  let make = _children: ReasonReact.component(unit, 'a, unit) => {
    ...component,
    didMount: self => {
      open Webapi.Dom;

      let listener = event => {
        let event = Auth.toStorageEvent(event);
        let key = event->StorageEvent.key;
        if (key == Auth.userIdKey) {
          let newValue = event->StorageEvent.newValue->Utils.toNullable;
          Store.broadcast(newValue->Js.Nullable.toOption);
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
  let make = children => {
    ...component,
    initialState: () => getCurrentState(),
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

  let make = children => {
    ...component,
    render: _self =>
      <IsAuthenticated>
        ...(
             state =>
               switch (state) {
               | None => children(None)
               | Some(userId) =>
                 open GqlUserInfo;
                 let query = UserInfoGql.make(~userId, ());
                 <UserInfoComponent variables=query##variables>
                   ...(
                        ({result}) =>
                          switch (result) {
                          | Loading => children(None)
                          | Error(_) => children(None)
                          | Data(response) =>
                            response##user_public
                            ->(
                                arrayFirst(~empty=children(None), ~render=user =>
                                  children(Some((user, userId)))
                                )
                              )
                          }
                      )
                 </UserInfoComponent>;
               }
           )
      </IsAuthenticated>,
  };
};
