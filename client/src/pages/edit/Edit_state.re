type filename = string;

type file_type =
  | RE
  | REI
  | ML
  | MLI
  | JS;

type file = {
  code: string,
  file_type,
};

/* TODO: fix me */
let file_type_from_name = _filename => RE;

type state = {
  files: Belt.Map.String.t(file),
  active_file: filename,
};

type action =
  | File_open(filename)
  | File_rename(filename, file)
  | File_update(filename, string);

let reducer = (action, state) => {
  ReactUpdate.(
    switch (action) {
    | File_open(filename) => Update({...state, active_file: filename})
    | File_rename(filename, new_filename) =>
      Update({
        ...state,
        files: state.files->Belt.Map.String.set(filename, new_filename),
      })
    | File_update(filename, new_value) =>
      Update({
        ...state,
        files:
          state.files
          ->Belt.Map.String.update(
              filename,
              fun
              | None =>
                Some({
                  code: new_value,
                  file_type: file_type_from_name(filename),
                })
              | Some(file) => Some({...file, code: new_value}),
            ),
      })
    }
  );
};
