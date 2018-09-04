module Unload: {
  type message = ref(option(string));
  type callback = option(message => unit);

  let register: (message => unit) => unit;
  let unregister: unit => unit;

  module Provider: {
    let make:
      React.childless =>
      React.component(unit, ReasonReact.noRetainedProps, unit);
  };
};

let redirect: string => unit;
let push: Route.t => unit;

let pushSilent: Route.t => unit;
let replaceSilent: Route.t => unit;

module Link: {
  let make:
    (
      ~route: Route.t,
      ~id: string=?,
      ~title: string=?,
      ~className: string=?,
      ~popup: bool=?,
      ~role: string=?,
      array(ReasonReact.reactElement)
    ) =>
    ReasonReact.component(
      ReasonReact.stateless,
      ReasonReact.noRetainedProps,
      ReasonReact.actionless,
    );
};
