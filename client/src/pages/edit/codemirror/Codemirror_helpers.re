open! CodeMirror;

let make_loc = err => {
  let loc = err.Error.loc->Loc.erase_option;

  let start_col = Loc.start_col(loc);
  let end_col = Loc.end_col(loc);

  let (start_col, end_col) =
    if (Loc.is_empty(loc)) {
      let start_col = start_col == 0 ? 0 : start_col - 1;
      let end_col = start_col == 0 ? end_col + 1 : end_col;
      (start_col, end_col);
    } else {
      (start_col, end_col);
    };

  let from_pos = Position.make(~line=Loc.start_line(loc), ~ch=start_col, ());
  let to_pos = Position.make(~line=Loc.end_line(loc), ~ch=end_col, ());

  (from_pos, to_pos);
};

let clean_error_message = msg => {
  let split_point = msg |> Js.String.indexOf("Error:");

  msg |> Js.String.sliceToEnd(~from=split_point);
};

let make_error_marker = err => {
  let (from, to_) = make_loc(err);

  LintOptions.annotation(
    ~from,
    ~to_,
    ~message=err.Error.message,
    ~severity="error",
  );
};

let make_warning_marker = err => {
  let (from, to_) = make_loc(err);
  LintOptions.annotation(
    ~from,
    ~to_,
    ~message=err.Error.message,
    ~severity="warning",
  );
};
