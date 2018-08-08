open Utils;
module Auth = Auth.Auth;
let getCurrentState = Auth.getUserId;

type state = option(string);
type action =
  | UpdateAuthState(state);

module Store = {
  let subscribers = ref([]);
  let subscribe = subscriber => subscribers := [subscriber, ...subscribers^];
  let broadcast = newValue =>
    Belt.List.forEach(subscribers^, subscriber =>
      subscriber(UpdateAuthState(newValue))
    );
};

module Provider = {
  let component = ReasonReact.reducerComponent("AuthStatus.Provider");

  let make = children: ReasonReact.component(unit, 'a, unit) => {
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
    render: _self => children->ReasonReact.array,
  };
};

module IsAuthenticated = {
  let component = ReasonReact.reducerComponent("AuthStatus.IsAuthenticated");
  let make = children => {
    ...component,
    initialState: () => getCurrentState(),
    reducer: (action, _state) =>
      switch (action) {
      | UpdateAuthState(state) => ReasonReact.Update(state)
      },
    didMount: ({send}) => Store.subscribe(send)->ignore,
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
