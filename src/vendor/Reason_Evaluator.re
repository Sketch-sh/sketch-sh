module Worker = {
  type t;
};

module RtopWorker = {
  [@bs.new] [@bs.module] external make : unit => Worker.t = "./rtop.worker.js";
};

module Comlink = {
  type t;
  [@bs.module] external comlink : t = "comlink";
  [@bs.send] external proxy : (t, Worker.t) => 'a = "proxy";
};

[@bs.deriving abstract]
type js_executeResult = {
  evaluate: string,
  stderr: string,
  stdout: string,
};

[@bs.deriving abstract]
type rtop = {
  execute: (. string) => Js.Promise.t(js_executeResult),
  reset: (. unit) => Js.Promise.t(unit),
  reasonSyntax: (. unit) => Js.Promise.t(unit),
  mlSyntax: (. unit) => Js.Promise.t(unit),
  parseError: (. string, string) => Js.Promise.t(string),
};
let worker = RtopWorker.make();

let rtop: rtop = Comlink.comlink |. Comlink.proxy(worker);

let reset = rtop |. resetGet;
let reasonSyntax = rtop |. reasonSyntaxGet;
let mlSyntax = rtop |. mlSyntaxGet;
let js_execute = rtop |. executeGet;
let js_parseError = rtop |. parseErrorGet;

type executeResult = {
  evaluate: option(string),
  stderr: option(string),
  stdout: option(string),
};

let emptyStringToOption =
  fun
  | "" => None
  | str => Some(str);

let execute: string => Js.Promise.t(executeResult) =
  code =>
    Js.Promise.(
      js_execute(. code)
      |> then_(result =>
           resolve({
             evaluate:
               result |. evaluateGet |. Js.String.trim |. emptyStringToOption,
             stderr:
               result |. stderrGet |. Js.String.trim |. emptyStringToOption,
             stdout:
               result |. stdoutGet |. Js.String.trim |. emptyStringToOption,
           })
         )
    );

let parseError = (~content, ~error) =>
  js_parseError(.
    content,
    error |> Js.String.replace({|File ""|}, {|File "_none_"|}),
  );
