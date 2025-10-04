type t =
  | V4_06
  | V4_13_1
  | V5_3_0;

let current = V5_3_0;

let ofDbString =
  fun
  | "4.06" => V4_06
  | "4.13.1" => V4_13_1
  | "5.3.0" => V5_3_0
  | _ => V4_06;

let toDbString =
  fun
  | V4_06 => "4.06"
  | V4_13_1 => "4.13.1"
  | V5_3_0 => "5.3.0";

let toUiString =
  fun
  | V4_06 => "4.06"
  | V4_13_1 => "4.13.1"
  | V5_3_0 => "5.3.0";
