type t =
  | V4_06
  | V4_13_1;

let current = V4_13_1;

let ofDbString =
  fun
  | "4.06" => V4_06
  | "4.13.1" => V4_13_1
  | _ => V4_06;

let toDbString =
  fun
  | V4_06 => "4.06"
  | V4_13_1 => "4.13.1";

let toUiString =
  fun
  | V4_06 => "4.06"
  | V4_13_1 => "4.13.1";
