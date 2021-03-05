module Worker = {
  type t;

  type self;
  [@bs.val] external self: self = "self";

  [@bs.val] external importScripts: string => unit = "importScripts";
};

module Comlink = {
  type t;
  [@bs.module] external comlink: t = "comlink";
  [@bs.send] external proxy: (t, Worker.t) => 'a = "proxy";
  [@bs.send] external expose: (t, 'a, Worker.self) => unit = "expose";
};
