module Parse = Engine_native.Types.Parse;
module Execute = Engine_native.Types.Execute;
/*
 * |--- executed ---|--- parsed_and_executable ---|--- unparsable/new code ---|
 *                  ^ last_executed_line
 */
type line_range = (int, int);
type gutter_status = {
  executed: option(line_range),
  parsed_and_executable: option(line_range),
};

let derive_execution_status =
    (arr_parse: list(Parse.parse_result), last_executed_line) => {
  let executed = last_executed_line->Belt.Option.map(line => (0, line));

  switch (arr_parse) {
  | [] => {executed, parsed_and_executable: None}
  | [last_phr, ..._xs] =>
    let last_parsed_line = last_phr.Parse.loc->Loc.erase_option->Loc.end_line;

    {
      executed,
      parsed_and_executable:
        Some((
          last_executed_line
          ->Belt.Option.map(line => line + 1)
          ->Belt.Option.getWithDefault(0),
          last_parsed_line,
        )),
    };
  };
};

type state = {
  last_executed_line: option(int),
  parse_error: option(Error.t),
  parse_success: list(Parse.parse_result),
  exec_msg: Belt.Map.Int.t(Execute.message),
};

type action =
  | Out_parse(string)
  | In_parse(Parse.message)
  | Editor_changed(array(Loc.t))
  | Add_exec_messages(Execute.message)
  | State_reset
  | Execute(int);

let reducer = (action, state) => {
  ReactUpdate.(
    switch (action) {
    | Out_parse(code) =>
      SideEffects(
        ({send}) => {
          let decoded_result = Engine_native.parse(code);
          switch (decoded_result) {
          | Ok(parse_result) => send(In_parse(parse_result))
          | Error(decco_error) =>
            Js.log2(
              "Ooops! Error while decoding value from worker, looks like types is out of sync",
              decco_error,
            )
          };

          None;
        },
      )
    | In_parse(parse_info) =>
      switch (parse_info) {
      | Error(parse_error) =>
        let err_start_line = parse_error.loc->Loc.erase_option->Loc.start_line;

        Update({
          ...state,
          parse_error: Some(parse_error),
          parse_success:
            state.parse_success
            ->Belt.List.keep(phr =>
                err_start_line > phr.Parse.loc->Loc.erase_option->Loc.end_line
              ),
        });
      | Ok(parse_success) =>
        Update({...state, parse_error: None, parse_success})
      }
    | Editor_changed(loc) =>
      // TODO: handle multiple loc
      let loc = loc[0];
      let changed_start_line = loc->Loc.start_line;
      let should_reset =
        switch (state.last_executed_line) {
        | None => false
        | Some(last_executed_line) => changed_start_line <= last_executed_line
        };
      if (should_reset) {
        SideEffects(
          ({send}) => {
            send(State_reset);
            None;
          },
        );
      } else {
        NoUpdate;
      };
    | State_reset => failwith("Unimplemented")
    | Add_exec_messages(message) =>
      Update({
        ...state,
        exec_msg:
          state.exec_msg->Belt.Map.Int.set(message.Execute.exec_id, message),
      })
    | Execute(to_id) =>
      SideEffects(
        ({send}) => {
          let complete = _ => ();
          let js_send = (
            fun
            | Ok(message) => send(Add_exec_messages(message))
            | Error(decco_error) =>
              Js.log2(
                "Ooops! Error while decoding value from worker, looks like types is out of sync",
                decco_error,
              )
          );

          Engine_native.execute(
            ~send={
              js_send;
            },
            ~complete,
            to_id,
          );
          None;
        },
      )
    }
  );
};
