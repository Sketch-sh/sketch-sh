let json_of_jsonb = a => a;
let decodeBlockData =
  fun
  | None => None
  | Some(json) => Some(Editor_Types.JsonDecode.decode(json));

external hackRefetchQueries:
  {
    .
    "query": string,
    "variables": Js.t('a),
  } =>
  string =
  "%identity";
