type success = {
  code: string,
  warnings: string,
};

module InternalResult = {
  type t = {. "_type": Js.nullable(string)};

  type internalSuccess = {. "js_code": string};

  type internalError = {
    .
    "js_error_msg": string,
    "row": int,
    "column": int,
    "endRow": int,
    "endColumn": int,
    "text": string,
  };

  external unsafeFromJson: 'a => t = "%identity";
  external unsafeAsSuccess: t => internalSuccess = "%identity";
  external unsafeAsError: t => internalError = "%identity";

  let toResult = jsObj =>
    switch (Js.Nullable.toOption(jsObj##_type)) {
    | Some("error") =>
      let error = jsObj |> unsafeAsError;
      Belt.Result.Error({
        Error.message: error##text,
        loc:
          Some({
            loc_start: {
              line: error##row,
              col: error##column,
            },
            loc_end: {
              line: error##endRow,
              col: error##endColumn,
            },
          }),
      });
    | _ =>
      Belt.Result.Ok({
        code: (jsObj |> unsafeAsSuccess)##js_code,
        warnings: "",
      })
    };
};

[@bs.val] [@bs.scope "ocaml"]
external ocaml_compile: string => string = "compile";
[@bs.val] [@bs.scope "reason"]
external reason_compile: string => string = "compile";

type compile_result = Belt.Result.t(success, Error.t);

let compile = (compiler, code): compile_result => {
  let json = compiler(code);

  json
  |> InternalResult.unsafeFromJson
  |> InternalResult.toResult
  |> Belt.Result.(
       fun
       | Ok({code}) => Ok({code, warnings: ""})
       | Error(error) => Error(error)
     );
};

let ocaml_compile = compile(ocaml_compile);
let reason_compile = compile(reason_compile);
