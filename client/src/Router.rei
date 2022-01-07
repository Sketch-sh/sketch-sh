/*
 * Unload
 * This is a helper to prevent route transition
 * when there is unsaved content
 * Example:
 *  let derivedMessageFromState: state => option(string) =
 *  let unloadHandler = (message, self) => {
 *    message := derivedMessageFromState(self.ReasonReact.state)
 *    ();
 *  };
 *  Router.Unload.register(self.handle(unloadHandler));
 */
module Unload: {
  type message = ref(option(string));
  type callback = option(message => unit);

  let register: (message => unit) => unit;
  let unregister: unit => unit;

  /*
   * Provider
   * Put this in any component
   */
  module Provider: {
    let make:
      ReactOld.childless =>
      ReactOld.component(unit, ReactOld.noRetainedProps, unit);
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
      array(ReactOld.reactElement)
    ) =>
    ReactOld.component(
      ReactOld.stateless,
      ReactOld.noRetainedProps,
      ReactOld.actionless,
    );
};
