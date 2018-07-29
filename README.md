# Reason online REPL - rtop

## How does it work?

This project uses [Reason online evaluator](https://github.com/thangngoc89/reason-online-evaluator)
for evaluating the input. You can see the binding to the library [here](blob/master/src/vendor/Reason_Evaluator.re)

## How to build?

- Clone this project

```sh
https://github.com/thangngoc89/rtop-ui
```

- Install dependencies

```sh
npm install
```

- Run an initial bsb build

```sh
npm run bs:build
```

- Start webpack development server

```sh
npm start
```

- Run Bucklescript in watch mode (another tab)

```sh
npm run bs:start
```

> Tips: You don't need to do this if you're using VSCode with VSCode ReasonML plugin
> just hit save and bsb will compile everything for you
> More information here: https://github.com/reasonml-editor/vscode-reasonml/#bsb

- Do cool things :cool:

## LICENSE

Apache 2.0. See the included LICENSE file for more information
