/* open Core; */

type parse_id = int;

type parse_error = {
  loc: option(Core.Loc.t),
  message: string,
};

type parse_result = {
  id: parse_id,
  loc: option(Core.Loc.t),
};

type state_phr = {
  sphr_id: parse_id,
  sphr_loc: option(Core.Loc.t),
  sphr_phrase: Parsetree.toplevel_phrase,
};
type state = {
  phrs: list(state_phr),
  last_executed_phr: option(parse_id),
};

let initial_state = {phrs: [], last_executed_phr: None};
let state = ref(initial_state);
let reset_state = () => state := initial_state;

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
      Toploop.parse_use_file^(lexbuf)
      |> List.fold_left(
           (acc, phrase) => {
             let loc = Location_util.location_of_toplevel_phrase(phrase);
             [{id: Hashtbl.hash(phrase), loc}, ...acc];
           },
           [],
         )
      |> (a => Ok(a));
    }
  ) {
  | Syntaxerr.Error(err) =>
    let loc = Syntaxerr.location_of_error(err) |> Core.Loc.toLocation;
    Syntaxerr.report_error(Format.str_formatter, err);
    let errorString = Format.flush_str_formatter();
    Error({loc, message: errorString});
  | Reason_syntax_util.Error(loc, Syntax_error(message)) =>
    let loc = loc |> Core.Loc.toLocation;
    Error({loc, message});
  | Reason_lexer.Error(err, loc) =>
    let reportedError =
      Location.error_of_printer(loc, Reason_lexer.report_error, err);

    let loc = reportedError.loc |> Core.Loc.toLocation;
    Error({loc, message: reportedError.msg});
  | exn =>
    switch (Location.error_of_exn(exn)) {
    | Some(`Ok(err)) =>
      Location.report_error(Format.str_formatter, err);
      let message = Format.flush_str_formatter();

      Error({
        loc: Report.extractLocation(exn) |> Util.Option.flatMap(Core.Loc.toLocation),
        message,
      });

    | Some(`Already_displayed)
    | None => Error({loc: None, message: Printexc.to_string(exn)})
    }
  };
};

let execute = execute_to_phrase => {
  failwith("Unimplemented");
};
