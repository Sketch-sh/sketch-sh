type state = {
  displayLoading: bool,
  timeoutId: option(Js.Global.timeoutId),
};

type action =
  | SetTimeoutId(option(Js.Global.timeoutId))
  | ChangeLoadingDisplay(bool);

let reducer = (state, action) => {
  switch (action) {
  | SetTimeoutId(timeoutId) => {...state, timeoutId}
  | ChangeLoadingDisplay(displayLoading) => {...state, displayLoading}
  };
};

let clearTimeout =
  fun
  | None => ()
  | Some(timeoutId) => Js.Global.clearTimeout(timeoutId);

[@react.component]
let make = (~delayMs=1000, ~loading: bool, children) => {
  let (state, dispatch) =
    React.useReducer(reducer, {displayLoading: loading, timeoutId: None});

  React.useEffect1(
    () => {
      if (loading) {
        /*
         * Switching from not loading to loading
         * Set new timeout to display spinner
         */
        dispatch @@
        SetTimeoutId(
          Some(
            Js.Global.setTimeout(
              () => dispatch(ChangeLoadingDisplay(true)),
              delayMs,
            ),
          ),
        );
      } else {
        /*
         * Switching from loading to not loading
         * Stop spinner
         */
        dispatch @@ ChangeLoadingDisplay(false);
      };
      Some(() => clearTimeout(state.timeoutId));
    },
    [|loading|],
  );

  children(state.displayLoading);
};
