open Editor_Types;

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

    let langDecoder: Js.Json.t => lang =
      json => json |> string |> stringToLang;

    external jsonIdentity: Js.Json.t => Js.Json.t = "%identity";

    let internalLinkDecoder: Js.Json.t => Link.internalLink =
      json => {
        revision_at: json |> field("timestamp", jsonIdentity),
        sketch_id: json |> field("id", string),
        name: json |> field("name", string),
        lang: json |> field("lang", langDecoder),
        code: json |> field("code", string),
      };

    let linkDecoder: Js.Json.t => Link.link =
      json => {
        let kind = json |> field("kind", string);
        switch (kind) {
        | "internal" => Internal(json |> field("value", internalLinkDecoder))
        | _ => failwith("There is no such link kind.")
        };
      };

    let decode: Js.Json.t => (lang, array(Link.link), array(Block.block)) =
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

    let internalLinkEncoder: Link.internalLink => Js.Json.t =
      internalLink =>
        object_([
          ("id", string(internalLink.sketch_id)),
          ("name", string(internalLink.name)),
          ("lang", internalLink.lang |> langToString |> string),
          ("timestamp", internalLink.revision_at),
          ("code", string(internalLink.code)),
        ]);

    let linkEncoder: Link.link => Js.Json.t =
      link =>
        switch (link) {
        | Internal(internalLink) =>
          object_([
            ("kind", "internal" |> string),
            ("value", internalLink |> internalLinkEncoder),
          ])
        | _ => failwith("There are no external links yet.")
        };

    let encode: (lang, array(Link.link), array(Block.block)) => Js.Json.t =
      (lang, links, blocks) =>
        object_([
          ("lang", lang |> langToString |> string),
          ("links", links->mapToJsonArray(linkEncoder)),
          ("blocks", blocks->mapToJsonArray(blockEncoder)),
        ]);
  };

  let decode = JsonDecode.decode;
  let encode = JsonEncode.encode;
};
