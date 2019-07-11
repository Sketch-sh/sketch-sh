/* open Core; */

type parse_id = int;

type parse_result = {
  id: parse_id,
  loc: option(Core.Loc.t),
};

type parse_error = {
  loc: option(Core.Loc.t),
  message: string,
};

type state;

let initial_state: state;
let reset_state: unit => unit;

let parse: string => result(list(parse_result), parse_error);

let execute: parse_id => unit;
