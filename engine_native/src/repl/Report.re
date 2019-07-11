open Util;
module Loc = Core.Loc;
module Evaluate = Core.Evaluate;

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

let _debugPrintError = exn => {
  let b = Buffer.create(256);
  let ppf = Format.formatter_of_buffer(b);
  Errors.report_error(ppf, exn);
  Format.pp_print_flush(ppf, ());
  Buffer.contents(b);
};

let reportError: exn => Core.Evaluate.error =
  exn => {
    let locFromExn = extractLocation(exn) |> Option.flatMap(Loc.toLocation);

    switch (Location.error_of_exn(exn)) {
    | None
    | Some(`Already_displayed) => {
        Evaluate.errLoc: locFromExn,
        Evaluate.errMsg: Printexc.to_string(exn),
        Evaluate.errSub: [],
      }
    | Some(`Ok({loc, msg, sub, if_highlight})) => {
        Evaluate.errLoc:
          switch (locFromExn) {
          | None => loc |> Loc.toLocation
          | Some(loc) => Some(loc)
          },
        Evaluate.errMsg: msg,
        Evaluate.errSub:
          sub
          |> List.map(({Location.loc, msg, _}) =>
               (loc |> Loc.toLocation, msg)
             ),
      }
    };
  };
