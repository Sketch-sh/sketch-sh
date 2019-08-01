let get = key => {
  let env = Node.Process.process##env;
  switch (env->Js.Dict.get(key)) {
  | Some(value) => Some(value)
  | None =>
    let file_key = key ++ "_FILE";
    switch (env->Js.Dict.get(file_key)) {
    | Some(file_path) =>
      try (Some(Node.Fs.readFileSync(file_path, `utf8))) {
      | _exn => None
      }
    | None => None
    };
  };
};

let getWithDefault = (~key, ~default) => {
  Belt.Option.getWithDefault(get(key), default);
};

let getExn = key => {
  switch (get(key)) {
  | Some(secret) => secret
  | None =>
    Log.error2("Undefined ENV key", key);
    Node.Process.exit(1);

    failwith("unreachable code");
  };
};
