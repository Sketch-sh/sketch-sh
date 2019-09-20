open Types;
open Types.Execute;

let warnings = ref([]);

let () =
  Location.warning_printer :=
    (
      (loc, _fmt, w) => {
        switch (Warnings.report(w)) {
        | `Inactive => ()
        | `Active({Warnings.number, message, is_error, sub_locs}) =>
          /* TODO: Handle is_error */
          let warning = {
            Warning.loc: loc |> Location_util.normalize,
            message: Printf.sprintf("Warning %i: %s", number, message),
          };

          let sub_warnings =
            sub_locs
            |> List.map(((loc, message)) =>
                 {
                   Warning.loc: loc |> Location_util.normalize,
                   message: Printf.sprintf("Warning %i: %s", number, message),
                 }
               );

          warnings := List.concat([[warning], sub_warnings, warnings^]);
        };
      }
    );

let eval_result_bff = Buffer.create(256);
let ppf = Format.formatter_of_buffer(eval_result_bff);

let eval_phrase = phrase => {
  Env.reset_cache_toplevel();
  try (
    {
      let isOk = Toploop.execute_phrase(true, ppf, phrase);
      let message = Buffer.contents(eval_result_bff);
      Buffer.clear(eval_result_bff);
      Ok((isOk, message));
    }
  ) {
  | exn => Error(exn)
  };
};

let execute =
    (
      ~send: message => unit,
      ~complete: complete => unit,
      ~readStdout: (module ReadStdout.Sig),
      ~execute_to_id: Parse.id,
    ) => {
  warnings := [];

  module ReadStdout = (val readStdout: ReadStdout.Sig);

  let rec loop =
    fun
    | [] => complete(Exec_complete_success)
    | [phr, ...tl] => {
        let exec_id = phr.State.sphr_id;
        let exec_loc = phr.State.sphr_loc;

        /* Execute and capture result */
        let capture = ReadStdout.start();
        let eval_result = eval_phrase(phr.sphr_phrase);

        let exec_stdout = ReadStdout.stop(capture);

        let exec_phr_kind =
          switch (phr.sphr_phrase) {
          | Parsetree.Ptop_dir(_, _) => Directive
          | Parsetree.Ptop_def(_) => Code
          };

        let exec_content =
          switch (eval_result) {
          | Ok((true, content)) => Exec_phr_true(content)
          | Ok((false, content)) => Exec_phr_false(content)
          | Error(exn) =>
            let error = Report.report_error(exn);
            Exec_phr_exn(error);
          };

        send({
          exec_id,
          exec_loc,
          exec_phr_kind,
          exec_content,
          exec_warning: warnings^,
          exec_stdout,
        });

        /* Finally decide what to do next */
        /* TODO: set last executed phrase in State */
        switch (exec_content) {
        | Exec_phr_true(_)
        | Exec_phr_false(_) => loop(tl)
        | Exec_phr_exn(_) => complete(Exec_complete_error)
        };

        /* Reset warnings before go to new phrases */
        warnings := [];
      };

  let phrs = State.get_phrs_to_execute(execute_to_id);
  loop(phrs);
};
