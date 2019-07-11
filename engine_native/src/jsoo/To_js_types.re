[@deriving jsobject]
type position =
  Core.Loc.Position.t = {
    line: int,
    col: int,
  };

[@deriving jsobject]
type loc =
  Core.Loc.t = {
    loc_start: position,
    loc_end: position,
  };

[@deriving jsobject]
type parse_id = int;

[@deriving jsobject]
type parse_result =
  Repl.State.parse_result = {
    id: parse_id,
    loc: option(loc),
  };

[@deriving jsobject]
type parse_error =
  Repl.State.parse_error = {
    loc: option(loc),
    message: string,
  };

[@deriving jsobject]
[@jsobject.sum_type_as "object"]
type result('a, 'b) = Pervasives.result('a, 'b) = | Ok('a) | Error('b);

[@deriving jsobject]
type parse_final_result = result(list(parse_result), parse_error);
