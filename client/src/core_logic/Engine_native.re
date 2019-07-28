type result('a, 'b) = Belt.Result.t('a, 'b) = | Ok('a) | Error('b);

let result_encode = (encoder_a, encoder_b, v) =>
  (
    v =>
      switch (v) {
      | Ok(v0) => Js.Json.array([|Js.Json.string("Ok"), encoder_a(v0)|])
      | Error(v0) =>
        Js.Json.array([|Js.Json.string("Error"), encoder_b(v0)|])
      }
  )(
    v,
  )
and result_decode = (decoder_a, decoder_b, v) =>
  (
    v =>
      switch (Js.Json.classify(v)) {
      | Js.Json.JSONArray(jsonArr) =>
        let tagged = Js.Array.map(Js.Json.classify, jsonArr);
        switch (tagged[0]) {
        | Js.Json.JSONString("Ok") =>
          Js.Array.length(tagged) !== 2
            ? Decco.error(
                ~path=?None,
                "Invalid number of arguments to variant constructor",
                v,
              )
            : (
              switch (decoder_a(jsonArr[1])) {
              | Belt.Result.Ok(v0) => Belt.Result.Ok(Ok(v0))
              | Belt.Result.Error((_ as e: Decco.decodeError)) =>
                Belt.Result.Error({...e, path: "[0]" ++ e.path})
              }
            )
        | Js.Json.JSONString("Error") =>
          Js.Array.length(tagged) !== 2
            ? Decco.error(
                ~path=?None,
                "Invalid number of arguments to variant constructor",
                v,
              )
            : (
              switch (decoder_b(jsonArr[1])) {
              | Belt.Result.Ok(v0) => Belt.Result.Ok(Error(v0))
              | Belt.Result.Error((_ as e: Decco.decodeError)) =>
                Belt.Result.Error({...e, path: "[0]" ++ e.path})
              }
            )
        | _ =>
          Decco.error(~path=?None, "Invalid variant constructor", jsonArr[0])
        };
      | _ => Decco.error(~path=?None, "Not a variant", v)
      }
  )(
    v,
  );

module Types = {
  module Parse = {
    [@decco]
    type id = int;
    [@decco]
    type parse_result = {
      id,
      loc: option(Loc.t),
    };

    [@decco]
    type message = result(list(parse_result), Error.t);
  };

  module Execute = {
    [@decco]
    type complete =
      | Exec_complete_success
      | Exec_complete_error;

    [@decco]
    type phr_kind =
      | Directive
      | Code;

    /*
     * Toploop.execute_phrase has 2 ways of expressing errors
     * 1. Return false as the result of `Toploop.execute_phrase`
     *    - Ophr_exception (no ideas)
     *    - Incorrect directive names / directive argument types
     * 2. Throwing exception
     */
    [@decco]
    type exec_content =
      | Exec_phr_true(string)
      | Exec_phr_false(string)
      | Exec_phr_exn(Error.t);

    [@decco]
    type message = {
      exec_id: Parse.id,
      exec_loc: option(Loc.t),
      exec_phr_kind: phr_kind,
      exec_content,
      exec_warning: list(Warning.t),
      exec_stdout: string,
    };
  };

  module Syntax = {
    [@decco]
    type t =
      | RE
      | ML;
  };
};

[@bs.val] external parse: string => Js.Json.t = "parse";

let parse = code => {
  code->parse->Types.Parse.message_decode;
};

type js_callback = Js.Json.t => unit;

[@bs.val]
external execute: (. js_callback, js_callback, int) => unit = "execute";

let execute = (~send, ~complete, id) => {
  let send = json => {
    json->Types.Execute.message_decode->send;
  };
  let complete = json => {
    json->Types.Execute.complete_decode->complete;
  };

  execute(. send, complete, id);
};
