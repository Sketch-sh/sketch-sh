[@react.component]
let make = () => {
  React.useEffect0(() => {
    open Auth.Auth;
    Token.remove();
    UserId.remove();
    GqlClient.instance##resetStore()->ignore;
    Js.Global.setTimeout(() => Router.push(Route.Home), 0)->ignore;
  });

  "Logging out..."->ReasonReact.string;
};
