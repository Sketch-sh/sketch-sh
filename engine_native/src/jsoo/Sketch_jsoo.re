module Js = Js_of_ocaml.Js;
module Firebug = Js_of_ocaml.Firebug;
// open Util;

Js_of_ocaml_toplevel.JsooTop.initialize();
Repl.SyntaxControl.re();

let parse = input => {
  let input = Js.to_string(input);
  Repl.State.parse(input) |> To_js_types.jsobject_of_parse_final_result;
};

let () = Js.export_all([%js {val parse = parse}]);
