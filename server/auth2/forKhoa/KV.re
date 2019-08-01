open Pom;
let set = (key: string, value_: Js.Json.t): pom(unit) => {
  DB.Bricks.(
    insertInto("kv")
    ->value({"key": key, "value": value_})
    ->onConflict("key")
    ->doUpdate
  )
  |> DB.perform
  <$> ignore;
};

let get = (key: string): pom(Js.Json.t) => {
  DB.Bricks.(select("value")->from("kv")->whereEq("key", key))
  ->DB.perform
  ->Codec.decodePomLegacy(
      Json.Decode.oneOf([
        DecodeExt.first(Json.Decode.field("value", a => a)),
        _ => Js.Json.null,
      ]),
    );
};

let remove = (key: string): pom(unit) => {
  DB.Bricks.(delete()->from("kv")->whereEq("key", key))
  |> DB.perform
  <$> ignore;
};