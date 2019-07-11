type state = {
  loading: bool,
  displayLoading: bool,
  timeoutId: ref(option(Js.Global.timeoutId)),
};

type action =
  | ChangeLoading(bool)
  | ChangeLoadingDisplay(bool);

let component = ReasonReact.reducerComponent("UI_LoadingWrapper");

let clearTimeout =
  fun
  | None => ()
  | Some(timeoutId) => Js.Global.clearTimeout(timeoutId);

[@react.component]
let make = (~delayMs=1000, ~loading: bool, ~children, ()) => {
  let children = React.Children.toArray(children);
  ReactCompat.useRecordApi({
    ...component,
    initialState: () => {
      loading,
      displayLoading: loading,
      timeoutId: ref(None),
    },
    willUnmount: self => clearTimeout(self.state.timeoutId^),
    willReceiveProps: self =>
      if (loading != self.state.loading) {
        {...self.state, loading};
      } else {
        self.state;
      },
    didUpdate: ({oldSelf, newSelf: self}) =>
      if (oldSelf.state.loading != loading) {
        if (loading) {
          /*
           * Switching from not loading to loading
           * Set new timeout to display spinner
           */
          clearTimeout(self.state.timeoutId^);
          self.state.timeoutId :=
            Some(
              Js.Global.setTimeout(
                () => self.send(ChangeLoadingDisplay(true)),
                delayMs,
              ),
            );
        } else {
          /*
           * Switching from loading to not loading
           * Clean up and stop spinner
           */
          clearTimeout(self.state.timeoutId^);
          self.send(ChangeLoadingDisplay(false));
        };
      },
    reducer: (action, state) =>
      switch (action) {
      | ChangeLoadingDisplay(loading) =>
        ReasonReact.Update({...state, displayLoading: loading})
      | ChangeLoading(loading) => ReasonReact.Update({...state, loading})
      },
    render: ({state}) => children(state.displayLoading),
  });
};
