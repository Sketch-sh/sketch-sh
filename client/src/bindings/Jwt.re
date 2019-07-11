type t;

[@bs.send] external decode: (t, string) => Js.Json.t = "default";

let async: unit => Js.Promise.t(t) = () => Modules.import("jwt-decode");
