open Utils;

Modules.require("./UI_Notification.css");
type notice =
  | Open(string)
  | Close;
/*
 * TODO: Rewrite me to handle multiple notifications
 * This is a really bad implementation of notification system
 * It can only handle 1 notification at a time
 * But since we don't have that much notification, it doesn't really matter
 */

module Store = {
  let callback = ref(None);

  let registerCallback = func => callback := Some(func);

  let notify = message =>
    switch (callback^) {
    | None => ()
    | Some(func) => func(message)
    };
};

let notify: string => unit = Store.notify;

module Provider = {
  type state = {
    notice,
    timeoutId: ref(option(Js.Global.timeoutId)),
  };

  type action =
    | OpenNotice(notice)
    | CloseNotice;

  let component = ReasonReact.reducerComponent("UI_Notification");

  let openClassName = "UI_Notification--open";

  let make = _children => {
    ...component,
    initialState: () => {notice: Close, timeoutId: ref(None)},
    reducer: (action, state) =>
      switch (action) {
      | OpenNotice(notice) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, notice},
          (
            ({send, state}) => {
              switch (state.timeoutId^) {
              | None => ()
              | Some(id) => Js.Global.clearTimeout(id)
              };
              state.timeoutId :=
                Js.Global.setTimeout(() => send(CloseNotice), 4000)->Some;
            }
          ),
        )
      | CloseNotice => ReasonReact.Update({...state, notice: Close})
      },
    didMount: ({state, send, onUnmount}) => {
      Store.registerCallback(message => send(OpenNotice(Open(message))));
      onUnmount(() =>
        switch (state.timeoutId^) {
        | None => ()
        | Some(id) => Js.Global.clearTimeout(id)
        }
      );
    },
    render: ({state}) =>
      <>
        <div
          className=(
            "UI_Notification"
            ++ (
              switch (state.notice) {
              | Open(message) => " UI_Notification--open"
              | Close => ""
              }
            )
          )>
          (
            switch (state.notice) {
            | Open(message) => message->str
            | Close => React.null
            }
          )
        </div>
      </>,
  };
};
