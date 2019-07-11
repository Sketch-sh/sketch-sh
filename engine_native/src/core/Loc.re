module Position = {
  [@deriving show]
  type t = {
    line: int,
    col: int,
  };
};

[@deriving show]
type t = {
  loc_start: Position.t,
  loc_end: Position.t,
};

let toLocation: Location.t => option(t) =
  loc => {
    let (_file, start_line, start_char) =
      Location.get_pos_info(loc.loc_start);
    let (_, end_line, end_char) = Location.get_pos_info(loc.loc_end);
    /* The right way of handling ocaml syntax error locations. Do do this at home
         copied over from
         https://github.com/BuckleScript/bucklescript/blob/2ad2310f18567aa13030cdf32adb007d297ee717/jscomp/super_errors/super_location.ml#L73
       */
    let normalizedRange =
      /* Happens when loading cmi without implementation */
      if (start_char === (-1) || end_char === (-1)) {
        None;
            /* happens sometimes. Syntax error for example */
      } else if (start_line == end_line && start_char >= end_char) {
        /* in some errors, starting char and ending char can be the same. But
           since ending char was supposed to be exclusive, here it might end up
           smaller than the starting char if we naively did start_char + 1 to
           just the starting char and forget ending char */
        let same_char = start_char + 1;
        Some(((start_line, same_char), (end_line, same_char)));
      } else {
        /* again: end_char is exclusive, so +1-1=0 */
        Some((
          (start_line, start_char + 1),
          (end_line, end_char),
        ));
      };

    switch (normalizedRange) {
    | None => None
    | Some((
        (start_line, start_line_start_char),
        (end_line, end_line_end_char),
      )) =>
      /*
         Converting from 1-indexed to 0-indexed
         The above logic is so dangerous that I don't even want to touch it
       */
      let start_line = start_line - 1;
      let start_line_start_char = start_line_start_char - 1;
      let end_line = end_line - 1;
      let end_line_end_char = end_line_end_char - 1;
      Some({
        loc_start: {
          line: start_line,
          col: start_line_start_char,
        },
        loc_end: {
          line: end_line,
          col: end_line_end_char,
        },
      });
    };
  };
