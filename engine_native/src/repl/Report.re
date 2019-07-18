open Types;

let extractLocation =
  fun
  | Syntaxerr.Error(x) => Some(Syntaxerr.location_of_error(x))
  | Lexer.Error(_, loc)
  | Typecore.Error(loc, _, _)
  | Typetexp.Error(loc, _, _)
  | Typeclass.Error(loc, _, _)
  | Typemod.Error(loc, _, _)
  | Typedecl.Error(loc, _)
  | Translcore.Error(loc, _)
  | Translclass.Error(loc, _)
  | Translmod.Error(loc, _) => Some(loc)
  | Reason_syntax_util.Error(loc, _) => Some(loc)
  | Reason_lexer.Error(_err, loc) => Some(loc)
  | _ => None;

let report_error = exn => {
  Errors.report_error(Format.str_formatter, exn);
  let message = Format.flush_str_formatter();

  /* TODO: remove everything before "Error:" */
  let loc =
    extractLocation(exn) |> Util.Option.flatMap(Location_util.normalize);

  {Error.loc, Error.message};
};
