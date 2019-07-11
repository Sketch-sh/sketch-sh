let json_of_jsonb = a => a;

external hackRefetchQueries:
  {
    .
    "query": string,
    "variables": Js.t('a),
  } =>
  string =
  "%identity";
