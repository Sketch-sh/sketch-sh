open Editor_Types;

module V1: {
  let decode: Js.Json.t => (lang, array(Block.block));
  let encode: (lang, array(Block.block)) => Js.Json.t;
};
