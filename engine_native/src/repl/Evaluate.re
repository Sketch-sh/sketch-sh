open Util;
open Core.Evaluate;

let buffer = Buffer.create(256);
let ppf = Format.formatter_of_buffer(buffer);

/** {2 Communication} */;

/** {2 Communication} */;

let protocolSuccess = (~blockLoc, ~msg, ~warnings, ~stdout) =>
  Phrase({
    blockLoc,
    blockContent: BlockSuccess({msg: msg |> String.trim, warnings}),
    blockStdout: stdout,
  });

let protocolError = (~blockLoc, ~error, ~warnings, ~stdout) =>
  Phrase({
    blockLoc,
    blockContent: BlockError({error, warnings}),
    blockStdout: stdout,
  });

/** {2 Execution} */;

/** {2 Execution} */

let warnings = ref([]);

let () =
  Location.warning_printer :=
    (
      (loc, _fmt, w) => {
        switch (Warnings.report(w)) {
        | `Inactive => ()
        | `Active({Warnings.number, message, is_error, sub_locs}) =>
          warnings :=
            [
              {
                warnNumber: number,
                warnMsg: message,
                warnLoc: Core.Loc.toLocation(loc),
                warnSub:
                  sub_locs
                  |> List.map(((loc, msg)) =>
                       (Core.Loc.toLocation(loc), msg)
                     ),
              },
              ...warnings^,
            ]
        };
      }
    );

let rec last = (head, tail) =>
  switch (tail) {
  | [] => head
  | [head, ...tail] => last(head, tail)
  };

let locFromPhrase = {
  fun
  | Parsetree.Ptop_def([]) => None
  | Parsetree.Ptop_def([item, ...items]) => {
      let loc = {
        Location.loc_start: item.pstr_loc.Location.loc_start,
        Location.loc_end: last(item, items).pstr_loc.Location.loc_end,
        Location.loc_ghost: false,
      };
      Some(loc);
    }
  | Ptop_dir(_name, _argument) => None;
};

let eval_phrase = phrase => {
  Warnings.reset_fatal();
  Env.reset_cache_toplevel();
  try (
    {
      let isOk = Toploop.execute_phrase(true, ppf, phrase);
      let message = Buffer.contents(buffer);
      Buffer.clear(buffer);
      Ok((isOk, message));
    }
  ) {
  | exn => Error(exn)
  };
};

let eval =
    (
      ~send: Core.Evaluate.result => unit,
      ~complete: evalResult => unit,
      ~readStdout: (module ReadStdout.Sig),
      code: string,
    )
    : unit => {
  warnings := [];

  module ReadStdout = (val readStdout: ReadStdout.Sig);

  let rec loop =
    fun
    | [] => complete(EvalSuccess)
    | [phrase, ...tl] => {
        let blockLoc =
          locFromPhrase(phrase) |> Option.flatMap(Core.Loc.toLocation);

        /* Redirect stdout */
        let capture = ReadStdout.start();
        let evalResult = eval_phrase(phrase);
        /* Get stdout resut and return stdout back */
        let stdout = ReadStdout.stop(capture);

        switch (phrase, evalResult) {
        | (Parsetree.Ptop_dir(_name, _argument), Ok((_, msg))) =>
          /*
           * Directive result could be from anywhere :(
           * #help: stdout
           * #show_val 1: msg
           */
          switch (stdout, msg) {
          | ("", "") => ()
          | (msg, "")
          | ("", msg) => send(Directive(msg))
          | (msg1, msg2) => send(Directive(msg1 ++ "\n" ++ msg2))
          };
          loop(tl);
        | (Parsetree.Ptop_dir(_, _), Error(exn)) =>
          let extractedWarnings = warnings^;
          let {errMsg, _} = Report.reportError(exn);
          send(Directive(errMsg));
          /* Ignore directive errors */
          loop(tl);
        | (Parsetree.Ptop_def(_), Ok((true, ""))) => loop(tl)
        | (Parsetree.Ptop_def(_), Ok((true, msg))) =>
          let extractedWarnings = warnings^;
          send(
            protocolSuccess(
              ~blockLoc,
              ~msg,
              ~warnings=extractedWarnings,
              ~stdout,
            ),
          );
          loop(tl);
        | (Parsetree.Ptop_def(_), Ok((false, msg))) =>
          let extractedWarnings = warnings^;
          /* No ideas when this happens */
          send(
            protocolError(
              ~blockLoc,
              ~error={errMsg: msg, errLoc: None, errSub: []},
              ~warnings=extractedWarnings,
              ~stdout,
            ),
          );
          complete(EvalError);
        | (Parsetree.Ptop_def(_), Error(exn)) =>
          let extractedWarnings = warnings^;
          let error = Report.reportError(exn);
          send(
            protocolError(
              ~blockLoc,
              ~error,
              ~warnings=extractedWarnings,
              ~stdout,
            ),
          );
          complete(EvalError);
        };
        warnings := [];
      };

  try (
    {
      let filename = "//toplevel//";
      let lexbuf = Lexing.from_string(code);
      Location.init(lexbuf, filename);
      Location.input_name := filename;
      Location.input_lexbuf := Some(lexbuf);
      loop(Toploop.parse_use_file^(lexbuf));
    }
  ) {
  | Sys.Break => complete(EvalInterupted)
  | exn =>
    let extractedWarnings = warnings^;
    let error = Report.reportError(exn);
    send(
      protocolError(
        ~blockLoc=None,
        ~error,
        ~warnings=extractedWarnings,
        ~stdout="",
      ),
    );
    warnings := [];
    complete(EvalError);
  };
};
