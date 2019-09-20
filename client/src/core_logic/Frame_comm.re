[@bs.val] external window: Dom.window = "window";

type message_event('message) = {. "data": message_event_data('message)}
and message_event_data('message) = {
  .
  "_type": Js.Nullable.t(string),
  "payload": 'message,
};

type message_handler('message) = message_event('message) => unit;

[@bs.send]
external addMessageListener:
  (Dom.window, [@bs.as "message"] _, message_handler('message), bool) => unit =
  "addEventListener";

let to_host: Container_comm.to_host => unit = [%raw
  message => {
    {|
  window.top.postMessage({
    "type": "sketch",
    "payload": message
  }, "*")
 |};
  }
];
