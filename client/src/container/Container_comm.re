type filename = string;

type from_host =
  | Comm_file_update(filename, string)
  | Comm_set_entry(filename);

type to_host =
  | Comm_ready
  | Comm_send_log(ConsoleFeed.log_encoded);
