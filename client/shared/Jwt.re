type t;

[@bs.send]
external decode : (t, string) => Js.Nullable.t(Js.Json.t) = "decode";

let async: unit => Js.Promise.t(t) = () => Modules.import("jsonwebtoken");
