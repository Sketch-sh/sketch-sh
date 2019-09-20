/** Turn error into Monaco's marker */
let make_error_marker: Error.t => MonacoEditor.IMarkerData.t;

/** Turn warning into Monaco's marker */
let make_warning_marker: Warning.t => MonacoEditor.IMarkerData.t;
