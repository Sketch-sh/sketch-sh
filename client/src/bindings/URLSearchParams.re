type t;
[@bs.new] external js_make: string => t = "URLSearchParams";

let make = js_make;

[@bs.send] external js_get: (t, string) => Js.Nullable.t(string) = "get";

let get = (t, key) => Js.Nullable.toOption(js_get(t, key));

[@bs.send] external js_has: (t, string) => bool = "has";

let has = js_has;
