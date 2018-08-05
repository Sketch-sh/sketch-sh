let blocks = [|
  {"kind": "code", "value": Editor_Loremipsum.code1},
  {"kind": "code", "value": Editor_Loremipsum.code2},
|];

let blocks =
  if (Utils.env == "production") {
    Belt.Array.concat(
      [|{"kind": "text", "value": Editor_Loremipsum.text1}|],
      blocks,
    );
  } else {
    blocks;
  };

external toJson : 'a => Js.Json.t = "%identity";

let blocks =
  blocks
  |. Belt.Array.mapU((. data) => {"id": Utils.generateId(), "data": data})
  |. (blocks => {"blocks": blocks})
  |. toJson
  |. Editor_Types.JsonDecode.decode;

let component = ReasonReact.statelessComponent("Editor_Entry_Loader");

let make = _children => {
  ...component,
  render: _self =>
    <Editor_Note
      blocks
      title="ReasonML playground"
      onSave=((~title as _, ~data as _) => ())
    />,
};
