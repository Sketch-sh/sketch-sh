[@decco]
type t =
  Error.t = {
    loc: option(Loc.t),
    message: string,
  };
