/** Turn error into Codemirror's lint annotations */
let make_error_marker: Error.t => CodeMirror.LintOptions.annotation;

/** Turn warning into Codemirror's lint annotations */
let make_warning_marker: Warning.t => CodeMirror.LintOptions.annotation;
