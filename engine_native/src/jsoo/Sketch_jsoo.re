module Js = Js_of_ocaml.Js;
module Firebug = Js_of_ocaml.Firebug;

Js_of_ocaml_toplevel.JsooTop.initialize();
Repl.SyntaxControl.re();

let parse = input => {
  let input = Js.to_string(input);
  Repl.State.parse(input) |> To_js_types.Parse.jsobject_of_message;
};

let execute = (js_send, js_complete, id) => {
  let send = content => {
    js_send(To_js_types.Execute.jsobject_of_message(content));
  };

  let complete = complete_message => {
    js_complete(To_js_types.Execute.jsobject_of_complete(complete_message));
  };

  Repl.Execute.execute(
    ~send,
    ~complete,
    ~readStdout=(module ReadStdoutJs),
    ~execute_to_id=id,
  );
};

let () = Js.export_all([%js {val parse = parse; val execute = execute}]);
