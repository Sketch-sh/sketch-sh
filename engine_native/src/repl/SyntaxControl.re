open Types.Syntax;

let _activeSyntax = ref(RE);
let current = () => _activeSyntax^;

module ToploopBackup = {
  let parse_toplevel_phrase = Toploop.parse_toplevel_phrase^;
  let parse_use_file = Toploop.parse_use_file^;
  let print_out_value = Toploop.print_out_value^;
  let print_out_type = Toploop.print_out_type^;
  let print_out_class_type = Toploop.print_out_class_type^;
  let print_out_module_type = Toploop.print_out_module_type^;
  let print_out_type_extension = Toploop.print_out_type_extension^;
  let print_out_sig_item = Toploop.print_out_sig_item^;
  let print_out_signature = Toploop.print_out_signature^;
  let print_out_phrase = Toploop.print_out_phrase^;
};

let ml = () => {
  Toploop.parse_toplevel_phrase := ToploopBackup.parse_toplevel_phrase;
  Toploop.parse_use_file := ToploopBackup.parse_use_file;
  Toploop.print_out_value := ToploopBackup.print_out_value;
  Toploop.print_out_type := ToploopBackup.print_out_type;
  Toploop.print_out_class_type := ToploopBackup.print_out_class_type;
  Toploop.print_out_module_type := ToploopBackup.print_out_module_type;
  Toploop.print_out_type_extension := ToploopBackup.print_out_type_extension;
  Toploop.print_out_sig_item := ToploopBackup.print_out_sig_item;
  Toploop.print_out_signature := ToploopBackup.print_out_signature;
  Toploop.print_out_phrase := ToploopBackup.print_out_phrase;
  _activeSyntax := ML;
};

let re = () => {
  open Reason_toolchain.From_current;
  let wrap = (f, g, fmt, x) => g(fmt, f(x));
  Toploop.parse_toplevel_phrase :=
    Reason_util.correctly_catch_parse_errors(x =>
      Reason_toolchain.To_current.copy_toplevel_phrase(
        Reason_toolchain.RE.toplevel_phrase(x),
      )
    );
  Toploop.parse_use_file :=
    Reason_util.correctly_catch_parse_errors(x =>
      List.map(
        Reason_toolchain.To_current.copy_toplevel_phrase,
        Reason_toolchain.RE.use_file(x),
      )
    );
  Toploop.print_out_value :=
    wrap(copy_out_value, Reason_oprint.print_out_value);
  Toploop.print_out_type := wrap(copy_out_type, Reason_oprint.print_out_type);
  Toploop.print_out_class_type :=
    wrap(copy_out_class_type, Reason_oprint.print_out_class_type);
  Toploop.print_out_module_type :=
    wrap(copy_out_module_type, Reason_oprint.print_out_module_type);
  Toploop.print_out_type_extension :=
    wrap(copy_out_type_extension, Reason_oprint.print_out_type_extension);
  Toploop.print_out_sig_item :=
    wrap(copy_out_sig_item, Reason_oprint.print_out_sig_item);
  Toploop.print_out_signature :=
    wrap(List.map(copy_out_sig_item), Reason_oprint.print_out_signature);
  Toploop.print_out_phrase :=
    wrap(copy_out_phrase, Reason_oprint.print_out_phrase);
  _activeSyntax := RE;
};
