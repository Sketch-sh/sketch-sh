module Block = {
  type bcode = {
    bc_value: string,
    bc_firstLineNumber: int,
    bc_widgets: array(Editor_CodeBlockTypes.Widget.t),
  };

  type id = string;

  type blockData =
    | B_Code(bcode)
    | B_Text(string);

  type blockTyp =
    | BTyp_Code
    | BTyp_Text;

  type block = {
    b_id: id,
    b_data: blockData,
  };

  type focusChangeType =
    | FcTyp_EditorFocus
    | FcTyp_BlockFocusUp
    | FcTyp_BlockFocusDown;
};

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
    };
  let decode: Js.Json.t => array(Block.block) =
    json => json |> field("blocks", array(blockDecoder));
};

module JsonEncode = {
  open Json.Encode;

  let blockDataEncoder: Block.blockData => Js.Json.t =
    fun
    | B_Code({bc_value}) =>
      object_([("kind", string("code")), ("value", string(bc_value))])
    | B_Text(value) =>
      object_([("kind", string("text")), ("value", string(value))]);

  let blockEncoder: Block.block => Js.Json.t =
    ({b_id, b_data}) =>
      object_([("id", string(b_id)), ("data", blockDataEncoder(b_data))]);

  let encode: array(Block.block) => Js.Json.t =
    blocks =>
      object_([
        (
          "blocks",
          blocks
          |. Belt.Array.mapU((. block) => block |> blockEncoder)
          |. jsonArray,
        ),
      ]);
};
