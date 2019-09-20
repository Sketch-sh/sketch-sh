type worker;

[@bs.new] external make: string => worker = "Worker";

module type Config = {
  type topToWorkerData;
  type workerToTopData;
  let make: unit => worker;
};

module Make = (Config: Config) => {
  include Config;

  let make = Config.make;

  module Top = {
    [@bs.send]
    external postMessageToWorker: (worker, 'a) => unit = "postMessage";
    [@bs.set]
    external onMessageFromWorker:
      (worker, {. "data": Config.workerToTopData} => unit) => unit =
      "onmessage";
    [@bs.set]
    external onErrorFromWorker: (worker, Js.t('a) => unit) => unit =
      "onerror";
    [@bs.send] external terminate: worker => unit = "terminate";
  };

  module Worker = {
    type self;
    [@bs.val]
    external postMessageFromWorker: Config.workerToTopData => unit =
      "postMessage";
    [@bs.val] external self: self = "self";
    [@bs.val] external importScripts: string => unit = "importScripts";
    [@bs.set]
    external onMessageFromTop:
      (self, {. "data": Config.topToWorkerData} => unit) => unit =
      "onmessage";
  };
};
