[@bs.val] external env: string = "process.env.NODE_ENV";

let getDictString = (dict, key) =>
  switch (Js.Dict.get(dict, key)) {
  | Some(json) => Js.Json.decodeString(json)
  | _ => None
  };

let makeSuccessJson = () => {
  let json = Js.Dict.empty();
  Js.Dict.set(json, "success", Js.Json.boolean(true));
  Js.Json.object_(json);
};
