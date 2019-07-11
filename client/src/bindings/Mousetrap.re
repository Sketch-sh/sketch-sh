type t = {
  .
  [@bs.set] "stopCallback": (. Dom.keyboardEvent, Dom.element, string) => bool,
};

type handler = Dom.keyboardEvent => unit;

[@bs.new] [@bs.module] external make: unit => t = "mousetrap";

[@bs.new] [@bs.module]
external makeWithElement: Dom.element => t = "mousetrap";

[@bs.send] external bind: (t, string, handler) => unit = "bind";
[@bs.send] external unbind: (t, string) => unit = "unbind";
[@bs.send] external trigger: (t, string) => unit = "trigger";
[@bs.send] external reset: (t, unit) => unit = "reset";
