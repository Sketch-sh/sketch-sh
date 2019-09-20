open Types;

type state = {
  phrs: list(state_phr),
  last_executed_phr: option(Parse.id),
}
and state_phr = {
  sphr_id: Parse.id,
  sphr_loc: option(Loc.t),
  sphr_phrase: Parsetree.toplevel_phrase,
};

let rec state = ref(initial_state)
and initial_state = {phrs: [], last_executed_phr: None}
and reset_state = () => state := initial_state;

let update_state_phrs = phrs => {
  state := {...state^, phrs};
};

/*
 * TODO: compare parsed result with last one
 */
let parse = code => {
  let filename = "//toplevel//";
  let lexbuf = Lexing.from_string(code);
  Location.init(lexbuf, filename);
  Location.input_name := filename;
  Location.input_lexbuf := Some(lexbuf);
  try (
    {
      let phrs = Toploop.parse_use_file^(lexbuf);

      let state_phrs =
        phrs
        |> List.fold_left(
             (acc, phrase) => {
               let loc = Location_util.location_of_toplevel_phrase(phrase);
               [
                 {
                   sphr_id: Hashtbl.hash(phrase),
                   sphr_loc: loc,
                   sphr_phrase: phrase,
                 },
                 ...acc,
               ];
             },
             [],
           );
      update_state_phrs(state_phrs);

      let result_phrs =
        phrs
        |> List.fold_left(
             (acc, phrase) => {
               let loc = Location_util.location_of_toplevel_phrase(phrase);
               [{Parse.id: Hashtbl.hash(phrase), loc}, ...acc];
             },
             [],
           );
      Ok(result_phrs);
    }
  ) {
  | exn => Error(Report.report_error(exn))
  };
};

let get_phrs_to_execute = from_id => {
  let phrs = state^.phrs;

  let chop_to_id = (chop_id, list) => {
    let rec loop = list => {
      switch (list) {
      | [] => list
      | [xs, ...aux] =>
        if (xs.sphr_id == chop_id) {
          list;
        } else {
          loop(aux);
        }
      };
    };
    loop(list);
  };

  phrs
  |> chop_to_id(from_id)
  |> List.rev
  |> (
    list => {
      switch (state^.last_executed_phr) {
      | None => list
      | Some(last_executed_phr) => chop_to_id(last_executed_phr, list)
      };
    }
  );
};
