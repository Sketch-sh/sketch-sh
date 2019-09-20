module Compiler = Engine_bs_compiler;

type nonrec compile_result = Engine_bs_compiler.compile_result;

let reason_compile = Compiler.reason_compile;
let ocaml_compile = Compiler.ocaml_compile;
