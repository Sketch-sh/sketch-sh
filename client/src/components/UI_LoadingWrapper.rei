/*
 * This component helps to avoid flash of loading screen
 * By only renderng the loading indicator after delayMs
 */
type state = {
  displayLoading: bool,
  timeoutId: option(Js.Global.timeoutId),
};

type action =
  | SetTimeoutId(option(Js.Global.timeoutId))
  | ChangeLoadingDisplay(bool);

[@react.component]
let make:
  (~delayMs: int=?, ~loading: bool, ~children: bool => 'children) => 'children;
