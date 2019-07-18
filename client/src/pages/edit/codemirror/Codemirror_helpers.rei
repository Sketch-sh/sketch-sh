open Engine_native.Types;

/** Turn error into Monaco's marker */
let make_error_marker: Error.t => CodeMirror.LintOptions.annotation;

/** Turn warning into Monaco's marker */
let make_warning_marker: Warning.t => CodeMirror.LintOptions.annotation;
