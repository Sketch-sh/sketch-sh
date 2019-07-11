/* open Util; */

let rec last = (head, tail) =>
  switch (tail) {
  | [] => head
  | [head, ...tail] => last(head, tail)
  };

let location_of_toplevel_phrase = {
  fun
  | Parsetree.Ptop_def([]) => None
  | Parsetree.Ptop_def([item, ...items]) => {
      let loc = {
        Location.loc_start: item.pstr_loc.Location.loc_start,
        Location.loc_end: last(item, items).pstr_loc.Location.loc_end,
        Location.loc_ghost: false,
      };
      loc |> Core.Loc.toLocation;
    }
  | Ptop_dir(_name, _argument) => None;
};
