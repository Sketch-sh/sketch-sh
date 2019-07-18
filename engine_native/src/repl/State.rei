open Types;

type state_phr = {
  sphr_id: Parse.id,
  sphr_loc: option(Loc.t),
  sphr_phrase: Parsetree.toplevel_phrase,
};

let reset_state: unit => unit;
let parse: string => Parse.message;
let get_phrs_to_execute: Parse.id => list(state_phr);
