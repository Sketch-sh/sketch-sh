[@bs.val] external window: Dom.window = "window";

type message_event;
type message_handler = message_event => unit;

[@bs.send]
external addMessageListener:
  (Dom.window, [@bs.as "message"] _, message_handler, bool) => unit =
  "addEventListener";

window->addMessageListener(event => Js.log(event), false);
