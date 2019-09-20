[@decco]
type t = {
  loc_start: Position.t,
  loc_end: Position.t,
};

let dummy_pos = {Position.line: 0, col: 0};
let dummy_loc = {loc_start: dummy_pos, loc_end: dummy_pos};

let erase_option = loc => loc->Belt.Option.getWithDefault(dummy_loc);

let start_line = loc => loc.loc_start.Position.line;
let start_col = loc => loc.loc_start.Position.col;
let end_line = loc => loc.loc_end.Position.line;
let end_col = loc => loc.loc_end.Position.col;

let is_empty = loc => {
  start_line(loc) == end_line(loc) && start_col(loc) == end_col(loc);
};
// Check if a is before b or touching b
let is_before = (a, b) => {
  a->end_line < b->start_line
  || a->end_line == b->start_line
  && a->end_col < b->start_col;
};

let are_intersecting_or_touching = (a, b) =>
  if (is_before(a, b)) {
    false;
  } else if (is_before(b, a)) {
    false;
  } else {
    true;
  };

// Check if a is before b
let is_before_or_touching = (a, b) => {
  a->end_line < b->start_line
  || a->end_line == b->start_line
  && a->end_col <= b->start_col;
};

// Check if a and b are intersection
let are_intersecting = (a, b) =>
  if (is_before_or_touching(a, b)) {
    false;
  } else if (is_before_or_touching(b, a)) {
    false;
  } else {
    true;
  };

module Monaco = {
  let to_monaco = loc => {
    (
      start_line(loc) + 1,
      start_col(loc) + 1,
      end_line(loc) + 1,
      end_col(loc) + 1,
    );
  };

  let from_monaco = range => {
    {
      loc_start: {
        line: range##startLineNumber - 1,
        col: range##startColumn - 1,
      },
      loc_end: {
        line: range##endLineNumber - 1,
        col: range##endColumn - 1,
      },
    };
  };
};
