open Worker_Evaluator.Types;

[@bs.val] external js_execute: string => execResult = "evaluator.execute";
[@bs.val] external js_reset: unit => unit = "evaluator.reset";
[@bs.val] external mlSyntax: unit => unit = "evaluator.mlSyntax";
[@bs.val] external reSyntax: unit => unit = "evaluator.reasonSyntax";
[@bs.val]
external insertModule: (. string, string, string) => LinkResult.js_linkResult =
  "evaluator.insertModule";

/* Implementation */
[@bs.val]
external parseRE: string => Refmt.implementationAST = "refmt.parseRE";
[@bs.val]
external printRE: Refmt.implementationAST => string = "refmt.printRE";
[@bs.val]
external parseML: string => Refmt.implementationAST = "refmt.parseML";
[@bs.val]
external printML: Refmt.implementationAST => string = "refmt.printML";

/* Interface */
[@bs.val] external parseREI: string => Refmt.interfaceAST = "refmt.parseREI";
[@bs.val] external parseMLI: string => Refmt.interfaceAST = "refmt.parseMLI";
[@bs.val] external printREI: Refmt.interfaceAST => string = "refmt.printREI";
[@bs.val] external printMLI: Refmt.interfaceAST => string = "refmt.printMLI";
