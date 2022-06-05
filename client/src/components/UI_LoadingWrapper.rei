/*
 * This component helps to avoid flash of loading screen
 * By only renderng the loading indicator after delayMs
 */
type state = {
  displayLoading: bool,
  timeoutId: ref(option(Js.Global.timeoutId)),
};

type action =
  | ChangeLoading(bool)
  | ChangeLoadingDisplay(bool);

let make:
  (~delayMs: int=?, ~loading: bool, bool => ReasonReact.reactElement) =>
  ReasonReact.component(state, ReasonReact.noRetainedProps, action);
