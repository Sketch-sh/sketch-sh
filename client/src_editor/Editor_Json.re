open Editor_Types;
module Block = Editor_Types.Block;

module V1 = {
  module JsonDecode = {
    open Json.Decode;
    let blockDataDecoder: Js.Json.t => Block.blockData =
      json => {
        let kind = json |> field("kind", string);
        let value = json |> field("value", string);
        switch (kind) {
        | "code" =>
          B_Code({bc_value: value, bc_firstLineNumber: 1, bc_widgets: [||]})
        | "text" => B_Text(value)
        | _ => raise(Invalid_argument("Unexpected block type: " ++ kind))
        };
      };

    let blockDecoder: Js.Json.t => Block.block =
      json => {
        b_id: json |> field("id", string),
        b_data: json |> field("data", blockDataDecoder),
        b_deleted:
          json
          |> optional(field("deleted", bool))
          |> (
            fun
            | None => false
            | Some(bool) => bool
          ),
      };
    let linkDecoder: Js.Json.t => link =
      json => {
        note_id: json |> field("note_id", string),
        name: json |> field("name", string),
        revision_at: json |> field("revision_at", string),
        blocks: json |> field("blocks", array(blockDecoder)),
      };
    let langDecoder: Js.Json.t => lang =
      json => json |> string |> stringToLang;
    let decode: Js.Json.t => (lang, array(link), array(Block.block)) =
      json => (
        json
        |> optional(field("lang", langDecoder))
        |> (
          fun
          | None => RE
          | Some(lang) => lang
        ),
        json
        |> optional(field("links", array(linkDecoder)))
        |> (
          fun
          | None => [||]
          | Some(links) => links
        ),
        json |> field("blocks", array(blockDecoder)),
      );
  };

  module JsonEncode = {
    open Json.Encode;
    let mapToJsonArray: (array('a), 'a => Js.Json.t) => Js.Json.t =
      (entities, encoder) =>
        entities
        ->(Belt.Array.mapU((. entitity) => entitity |> encoder))
        ->jsonArray;

    let blockDataEncoder: Block.blockData => Js.Json.t =
      fun
      | B_Code({bc_value}) =>
        object_([("kind", string("code")), ("value", string(bc_value))])
      | B_Text(value) =>
        object_([("kind", string("text")), ("value", string(value))]);

    let blockEncoder: Block.block => Js.Json.t =
      ({b_id, b_data, b_deleted}) =>
        object_([
          ("id", string(b_id)),
          ("data", blockDataEncoder(b_data)),
          ("deleted", bool(b_deleted)),
        ]);

    let linkDecoder: link => Js.Json.t =
      ({note_id, name, revision_at, blocks}) =>
        object_([
          ("note_id", string(note_id)),
          ("name", string(name)),
          ("revision_at", string(revision_at)),
          ("blocks", blocks->mapToJsonArray(blockEncoder)),
        ]);

    let encode: (lang, array(link), array(Block.block)) => Js.Json.t =
      (lang, links, blocks) =>
        object_([
          ("lang", lang |> langToString |> string),
          ("links", links->mapToJsonArray(linkDecoder)),
          ("blocks", blocks->mapToJsonArray(blockEncoder)),
        ]);
  };
  let decode = JsonDecode.decode;
  let encode = JsonEncode.encode;
};
