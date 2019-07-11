type pos = {
  .
  "col": int,
  "line": int,
};

type loc = {
  .
  "loc_start": pos,
  "loc_end": pos,
};

let blank_pos: pos = {"col": 0, "line": 0};
let blank_loc: loc = {"loc_start": blank_pos, "loc_end": blank_pos};

let make_loc = (start_line, start_col, end_line, end_col) => {
  {
    "loc_start": {
      "line": start_line,
      "col": start_col,
    },
    "loc_end": {
      "line": end_line,
      "col": end_col,
    },
  };
};

// Check if a is before b or touching b
let is_before = (a, b) => {
  a##loc_end##line <
  b##loc_start##line
  ||
  a##loc_end##line ==
  b##loc_start##line
  &&
  a##loc_end##col <
  b##loc_start##col;
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
  a##loc_end##line <
  b##loc_start##line
  ||
  a##loc_end##line ==
  b##loc_start##line
  &&
  a##loc_end##col <=
  b##loc_start##col;
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
      loc##loc_start##line + 1,
      loc##loc_start##col + 1,
      loc##loc_end##line + 1,
      loc##loc_end##col + 1,
    );
  };

  let from_monaco = range => {
    {
      "loc_start": {
        "line": range##startLineNumber - 1,
        "col": range##startColumn - 1,
      },
      "loc_end": {
        "line": range##endLineNumber - 1,
        "col": range##endColumn - 1,
      },
    };
  };
};
