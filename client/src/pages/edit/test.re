[@bs.deriving jsConverter]
type t =
  | [@bs.as 1] Hint
  | [@bs.as 2] Info
  | [@bs.as 4] Warning
  | [@bs.as 8] Error;

let a = Hint;
let b = Info;
let c = Warning;
let d = Error;
