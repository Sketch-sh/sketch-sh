type filename = string;

type from_host =
  | Comm_file_update(filename, string)
  | Comm_set_entry(filename);
