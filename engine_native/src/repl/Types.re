module Position = {
  type t = {
    line: int,
    col: int,
  };
};

module Loc = {
  type t = {
    loc_start: Position.t,
    loc_end: Position.t,
  };
};

module Error = {
  type t = {
    loc: option(Loc.t),
    message: string,
  };
};

module Warning = {
  type t =
    Error.t = {
      loc: option(Loc.t),
      message: string,
    };
};

module Parse = {
  type id = int;
  type parse_result = {
    id,
    loc: option(Loc.t),
  };

  type message = result(list(parse_result), Error.t);
};

module Execute = {
  type complete =
    | Exec_complete_success
    | Exec_complete_error;

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
  type exec_content =
    | Exec_phr_true(string)
    | Exec_phr_false(string)
    | Exec_phr_exn(Error.t);

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
  type t =
    | RE
    | ML;
};
