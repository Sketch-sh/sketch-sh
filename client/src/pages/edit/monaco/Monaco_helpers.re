open MonacoEditor;

let make_marker = err => {
  let (startLineNumber, startColumn, endLineNumber, endColumn) =
    err.Error.loc->Loc.erase_option->Loc.Monaco.to_monaco;

  IMarkerData.make(
    ~message=err.Error.message,
    ~startLineNumber,
    ~startColumn,
    ~endLineNumber,
    ~endColumn=endColumn + 1,
  );
};

let make_error_marker = err => {
  make_marker(err, ~severity=MarkerSeverity.tToJs(MarkerSeverity.Error), ());
};

let make_warning_marker = err => {
  make_marker(err, ~severity=MarkerSeverity.(tToJs(Warning)), ());
};
