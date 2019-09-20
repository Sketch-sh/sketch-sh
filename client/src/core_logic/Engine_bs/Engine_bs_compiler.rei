type success = {
  code: string,
  warnings: string,
};
type compile_result = Belt.Result.t(success, Error.t);

let reason_compile: string => compile_result;
let ocaml_compile: string => compile_result;
