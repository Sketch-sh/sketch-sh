type state = bool;

let component = ReasonReact.reducerComponent("UI_WarningStagingServer");

let make = (_children): ReasonReact.component(state, 'a, state) => {
  ...component,
  initialState: () => false,
  reducer: (newState, _) => ReasonReact.Update(newState),
  didMount: ({send}) =>
    Webapi.Dom.(
      if (location->Location.host == Config.stagingHost) {
        send(true);
      }
    ),
  render: ({state: shouldDisplay}) =>
    shouldDisplay
      ? <div className="warning-staging-server">
          "WARNING: This is a staging server of ReasonML playground. All data will be reset every day"
          ->Utils.str
        </div>
      : ReasonReact.null,
};
