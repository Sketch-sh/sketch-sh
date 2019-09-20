module Js = Js_of_ocaml.Js;
module Firebug = Js_of_ocaml.Firebug;
open Util;

Js_of_ocaml_toplevel.JsooTop.initialize();
Repl.SyntaxControl.re();

let completion = input => {
  let input = Js.to_string(input);

  let (startPos, completions) = Completion.complete(input);

  %js
  {
    val completions =
      completions |> Array.of_list |> Array.map(fst >> Js.string) |> Js.array;
    val startPos = startPos
  };
};

let execute = (js_send, input) => {
  let input = Js.to_string(input);

  let send: Core.Evaluate.result => unit =
    content => {
      js_send(CoreTypes.Evaluate.result_to_js(content));
    };
  let complete = _ => {
    VendoredUTop.UTop_complete.reset();
  };

  Repl.Evaluate.eval(
    ~send,
    ~complete,
    ~readStdout=(module ReadStdoutJs),
    input,
  );
};

let () =
  Js.export_all([%js {val completion = completion; val execute = execute}]);
