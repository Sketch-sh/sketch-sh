/*
 * This component helps to avoid flash of loading screen
 * By only renderng the loading indicator after delayMs
 */
type state = {
  loading: bool,
  displayLoading: bool,
  timeoutId: ref(option(Js.Global.timeoutId)),
};

type action =
  | ChangeLoading(bool)
  | ChangeLoadingDisplay(bool);

[@react.component]
let make:
  (
    ~delayMs: int=?,
    ~loading: bool,
    ~children: bool => ReasonReact.reactElement,
    unit
  ) =>
  React.element;
