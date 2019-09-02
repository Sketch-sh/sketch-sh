# ReasonML playground

A note writing application for teaching and sharing ReasonML code with inline evaluation.

[![CircleCI](https://circleci.com/gh/Sketch-sh/sketch-sh/tree/master.svg?style=svg)](https://circleci.com/gh/Sketch-sh/sketch-sh/tree/master)

![](github/screenshot.png)

## How does it work?

This project uses [rtop-evaluator](https://github.com/Sketch-sh/rtop-evaluator) for evaluating the input. The evaluator loads refmt & js_of_ocaml compiler as a web worker in the browser. We're not using BuckleScript since it currently doesn't support toplevel evaluation. That means BuckleScript modules like `Js` are not available, and instead one has to run functions from the OCaml stdlib like `print_string`. Belt however works well with js_of_ocaml and support is coming soon!

## Contributions

See [DEVELOPING.md](https://github.com/Sketch-sh/sketch-sh/blob/master/DEVELOPING.md) for more information

## Contributors

### Code Contributors

This project exists thanks to all the people who contribute. [[Contribute](CONTRIBUTING.md)].
<a href="https://github.com/Sketch-sh/sketch-sh/graphs/contributors"><img src="https://opencollective.com/sketch-sh/contributors.svg?width=890&button=false" /></a>

### Financial Contributors

Become a financial contributor and help us sustain our community. [[Contribute](https://opencollective.com/sketch-sh/contribute)]

#### Individuals

<a href="https://opencollective.com/sketch-sh"><img src="https://opencollective.com/sketch-sh/individuals.svg?width=890"></a>

#### Organizations

Support this project with your organization. Your logo will show up here with a link to your website. [[Contribute](https://opencollective.com/sketch-sh/contribute)]

<a href="https://opencollective.com/sketch-sh/organization/0/website"><img src="https://opencollective.com/sketch-sh/organization/0/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/1/website"><img src="https://opencollective.com/sketch-sh/organization/1/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/2/website"><img src="https://opencollective.com/sketch-sh/organization/2/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/3/website"><img src="https://opencollective.com/sketch-sh/organization/3/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/4/website"><img src="https://opencollective.com/sketch-sh/organization/4/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/5/website"><img src="https://opencollective.com/sketch-sh/organization/5/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/6/website"><img src="https://opencollective.com/sketch-sh/organization/6/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/7/website"><img src="https://opencollective.com/sketch-sh/organization/7/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/8/website"><img src="https://opencollective.com/sketch-sh/organization/8/avatar.svg"></a>
<a href="https://opencollective.com/sketch-sh/organization/9/website"><img src="https://opencollective.com/sketch-sh/organization/9/avatar.svg"></a>

## LICENSE

Apache 2.0. See the included LICENSE file for more information
