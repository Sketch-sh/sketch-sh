type state = {
  loading: bool,
  displayLoading: bool,
  timeoutId: option(Js.Global.timeoutId),
};

type action =
  | ChangeLoading(bool)
  | SetTimeoutId(option(Js.Global.timeoutId))
  | ChangeLoadingDisplay(bool);

let reducer = (state, action) => {
  switch (action) {
  | ChangeLoading(loading) => {...state, loading}
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
    React.useReducer(
      reducer,
      {loading, displayLoading: loading, timeoutId: None},
    );

  React.useEffect0(
    () => {Some(() => {clearTimeout(state.timeoutId)})},
    [|loading|],
  );

  React.useEffect1(
    () => {
      dispatch @@
      ChangeLoading(loading != state.loading ? loading : state.loading);

      None;
    },
    [|loading|],
  );

  React.useEffect(() =>
    if (state.loading != loading) {
      if (loading) {
        /*
         * Switching from not loading to loading
         * Set new timeout to display spinner
         */
        clearTimeout(state.timeoutId);

        dispatch @@
        SetTimeoutId(
          Some(
            Js.Global.setTimeout(
              () => self.send(ChangeLoadingDisplay(true)),
              delayMs,
            ),
          ),
        );
      } else {
        /*
         * Switching from loading to not loading
         * Clean up and stop spinner
         */
        clearTimeout(state.timeoutId);
        dispatch @@ ChangeLoadingDisplay(false);
      };
    }
  );

  children(state.displayLoading);
};
