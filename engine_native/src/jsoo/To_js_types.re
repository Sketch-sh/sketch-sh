[@deriving jsobject]
[@jsobject.sum_type_as "object"]
type result('a, 'b) = Pervasives.result('a, 'b) = | Ok('a) | Error('b);

module T = Repl.Types;

module Position = {
  [@deriving jsobject]
  type t =
    T.Position.t = {
      line: int,
      col: int,
    };
};

module Loc = {
  [@deriving jsobject]
  type t =
    T.Loc.t = {
      loc_start: Position.t,
      loc_end: Position.t,
    };
};

module Error = {
  [@deriving jsobject]
  type t =
    T.Error.t = {
      loc: option(Loc.t),
      message: string,
    };
};

module Warning = {
  [@deriving jsobject]
  type t =
    T.Warning.t = {
      loc: option(Loc.t),
      message: string,
    };
};

module Parse = {
  [@deriving jsobject]
  type id = int;

  [@deriving jsobject]
  type parse_result =
    T.Parse.parse_result = {
      id,
      loc: option(Loc.t),
    };

  [@deriving jsobject]
  type message = result(list(parse_result), Error.t);
};

module Execute = {
  module E = T.Execute;

  [@deriving jsobject]
  type complete = E.complete = | Exec_complete_success | Exec_complete_error;

  [@deriving jsobject]
  type phr_kind = E.phr_kind = | Directive | Code;

  [@deriving jsobject]
  type exec_content =
    E.exec_content =
      | Exec_phr_true(string)
      | Exec_phr_false(string)
      | Exec_phr_exn(Error.t);

  [@deriving jsobject]
  type message =
    E.message = {
      exec_id: Parse.id,
      exec_loc: option(Loc.t),
      exec_phr_kind: phr_kind,
      exec_content,
      exec_warning: list(Warning.t),
      exec_stdout: string,
    };
};
