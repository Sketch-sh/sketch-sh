type state = {
  loading: bool,
  displayLoading: bool,
  timeoutId: ref(option(Js.Global.timeoutId)),
};

type action =
  | ChangeLoading(bool)
  | ChangeLoadingDisplay(bool);

let reducer = (state, action) =>
  switch (action) {
  | ChangeLoadingDisplay(loading) => {...state, displayLoading: loading}
  | ChangeLoading(loading) => {...state, loading}
  };

let clearTimeout =
  fun
  | None => ()
  | Some(timeoutId) => Js.Global.clearTimeout(timeoutId);

[@react.component]
let make = (~delayMs=1000, ~loading: bool, ~children) => {
  let (state, dispatch) =
    React.useReducer(
      reducer,
      {loading, displayLoading: loading, timeoutId: ref(None)},
    );

  let {loading, displayLoading, timeoutId} = state;

  React.useEffect(() => {
    clearTimeout(timeoutId^);
    None;
  });

  React.useEffect1(
    () => {
      if (loading) {
        /*
         * Switching from not loading to loading
         * Set new timeout to display spinner
         */
        clearTimeout(timeoutId^);
        timeoutId :=
          Some(
            Js.Global.setTimeout(
              () => dispatch @@ ChangeLoadingDisplay(true),
              delayMs,
            ),
          );
      } else {
        /*
         * Switching from loading to not loading
         * Clean up and stop spinner
         */
        clearTimeout(timeoutId^);
        dispatch @@ ChangeLoadingDisplay(false);
      };
      None;
    },
    [|loading|],
  );

  displayLoading ? children : React.null;
};
