type filename = string;

type file = {
  code: string,
  errors: array(Error.t),
  warnings: array(Error.t),
  compiled: option(string),
};

let make_file = code => {
  {code, compiled: None, errors: [||], warnings: [||]};
};

let get_extension: string => string =
  filename => [%raw
    {|  filename.slice((filename.lastIndexOf(".") - 1 >>> 0) + 2)|}
  ];

let get_extension = filename => {
  switch (get_extension(filename)) {
  | "" => None
  | ext => Some(ext)
  };
};

type state = {
  files: Belt.Map.String.t(file),
  active_file: filename,
};

type action =
  | File_open(filename)
  | File_rename(filename, file)
  | File_update(filename, string)
  | Compile_result(filename, Engine_bs.compile_result);

let debounce_compile =
  Debouncer.makeCancelable(((filename, code, send)) =>
    (
      switch (filename->get_extension) {
      | Some("re") =>
        Js.log("using reason compiler");
        Some(Engine_bs.reason_compile(code));
      | Some("ml") => Some(Engine_bs.ocaml_compile(code))
      | _ => None
      }
    )
    |> (
      fun
      | Some(c) => send(Compile_result(filename, c))
      | None => ()
    )
  );

let reducer = (action, state) => {
  ReactUpdate.(
    switch (action) {
    | File_open(filename) => Update({...state, active_file: filename})
    | File_rename(filename, new_filename) =>
      Update({
        ...state,
        files: state.files->Belt.Map.String.set(filename, new_filename),
      })
    | File_update(filename, new_value) =>
      UpdateWithSideEffects(
        {
          ...state,
          files:
            state.files
            ->Belt.Map.String.update(
                filename,
                fun
                | None =>
                  Some({
                    code: new_value,
                    compiled: None,
                    errors: [||],
                    warnings: [||],
                  })
                | Some(file) => Some({...file, code: new_value}),
              ),
        },
        ({send}) => {
          debounce_compile.invoke((filename, new_value, send));

          Some(() => debounce_compile.cancel());
        },
      )
    | Compile_result(filename, compile_result) =>
      Update({
        ...state,
        files:
          state.files
          ->Belt.Map.String.update(filename, __x =>
              Belt.Option.map(__x, file =>
                switch (compile_result) {
                | Ok({code}) => {
                    ...file,
                    compiled: Some(code),
                    errors: [||],
                    warnings: [||],
                  }
                | Error(error) => {
                    ...file,
                    compiled: None,
                    errors: [|error|],
                  }
                }
              )
            ),
      })
    }
  );
};
