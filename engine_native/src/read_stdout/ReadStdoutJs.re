module Sys_js = Js_of_ocaml.Sys_js;

type capture = Buffer.t;

let bff = Buffer.create(256);
Sys_js.set_channel_flusher(stdout, Buffer.add_string(bff));
let start = () => {
  bff |> Buffer.reset;
  bff;
};

let stop = Buffer.contents;
