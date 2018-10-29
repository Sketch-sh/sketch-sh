# ReasonML playground

A note writing application for teaching and sharing ReasonML code with inline evaluation.

[![CircleCI](https://circleci.com/gh/Sketch-sh/sketch-sh/tree/master.svg?style=svg)](https://circleci.com/gh/Sketch-sh/sketch-sh/tree/master)

![](github/screenshot.png)

## How does it work?

This project uses [rtop-evaluator](https://github.com/Sketch-sh/rtop-evaluator) for evaluating the input. The evaluator loads refmt & js_of_ocaml compiler as a web worker in the browser. We're not using BuckleScript since it currently doesn't support toplevel evaluation. That means BuckleScript modules like `Js` are not available, and instead one has to run functions from the OCaml stdlib like `print_string`. Belt however works well with js_of_ocaml and support is coming soon!

## Contributions

See [DEVELOPING.md](https://github.com/Sketch-sh/sketch-sh/blob/master/DEVELOPING.md) for more information

## LICENSE

Apache 2.0. See the included LICENSE file for more information
