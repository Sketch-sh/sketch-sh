# ReasonML playground

A note writing application for teaching and sharing ReasonML code with inline evaluation.

[![CircleCI](https://circleci.com/gh/Sketch-sh/sketch-sh/tree/master.svg?style=svg)](https://circleci.com/gh/Sketch-sh/sketch-sh/tree/master)

![](github/screenshot.png)

## How does it work?

This project uses [rtop-evaluator](https://github.com/Sketch-sh/rtop-evaluator) for evaluating the input. The evaluator loads refmt & js_of_oo compiler as a service worker in the browser. We're not using BuckleScript since it currently doesn't support toplevel evaluation. So you can't use BuckleScript libraries like `Js`, and instead will have to run OCaml stdlib functions like `print_string`. Belt however works well with js_of_oo and support is coming soon!

## Contributions

See [DEVELOPING.md](https://github.com/Sketch-sh/sketch-sh/blob/master/DEVELOPING.md) for more information

## LICENSE

Apache 2.0. See the included LICENSE file for more information
