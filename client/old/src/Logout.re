let component = ReasonReact.reducerComponent("Logout");

let make = (_children): ReasonReact.component(unit, 'a, unit) => {
  ...component,
  didMount: _ => {
    open Auth.Auth;
    Token.remove();
    UserId.remove();
    GqlClient.instance##resetStore();
    Js.Global.setTimeout(() => Router.push(Route.Home), 0)->ignore;
  },
  render: _self => "Logging out..."->ReasonReact.string,
};
