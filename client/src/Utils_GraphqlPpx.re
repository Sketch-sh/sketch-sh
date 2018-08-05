let json_of_jsonb = a => a;
let decodeBlockData =
  fun
  | None => None
  | Some(json) => Some(Editor_Types.JsonDecode.decode(json));
