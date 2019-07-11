open Toplevel.Types;

let timeoutSeconds = ref(180);
let toplevelWorker = ref(None);

module MapStr = Belt.MutableMap.String;

type callback =
  | ExecuteCallback(
      Belt.Result.t((list(linkResult), list(blockResult)), string) => unit,
    )
  | ExecuteEmbedCallback(
      Belt.Result.t(list(Worker_Types.blockData), string) => unit,
    )
  | RefmtCallback(Belt.Result.t(refmtOk, string) => unit)
  | LoadScriptCallback(unit => unit);

let ongoingCallbacks: MapStr.t((Js.Global.timeoutId, callback)) =
  MapStr.make();

let isReady = ref(false);
let readyCb = ref(None);

let workerListener = event => {
  let {w_id, w_message: message} = event##data;

  switch (ongoingCallbacks->MapStr.get(w_id)) {
  | None =>
    switch (w_id, message) {
    | ("ready", Ready) =>
      isReady := true;
      switch (readyCb^) {
      | None => ()
      | Some(cb) => cb()
      };
    | _ => ()
    }
  | Some((timeoutId, callback)) =>
    Js.Global.clearTimeout(timeoutId);
    switch (message) {
    | Ready => ()

    | ExecuteResult(result) =>
      switch (callback) {
      | ExecuteCallback(callback) => callback(result)
      | _ => ()
      }
    | RefmtResult(result) =>
      switch (callback) {
      | RefmtCallback(callback) => callback(result)
      | _ => ()
      }
    | ExecuteEmbedResult(result) =>
      switch (callback) {
      | ExecuteEmbedCallback(callback) => callback(result)
      | _ => ()
      }
    | LoadScriptResult =>
      switch (callback) {
      | LoadScriptCallback(callback) => callback()
      | _ => ()
      }
    };
    ongoingCallbacks->MapStr.remove(w_id);
  };
};

let getWorker = () =>
  switch (toplevelWorker^) {
  | None =>
    let newWorker = Toplevel.make();
    toplevelWorker := Some(newWorker);
    newWorker->Toplevel.Top.onMessageFromWorker(workerListener);
    newWorker->Toplevel.Top.onErrorFromWorker(Js.log);
    newWorker;
  | Some(worker) => worker
  };
let terminate = id => {
  ongoingCallbacks->MapStr.remove(id);
  getWorker()->Toplevel.Top.terminate;
  toplevelWorker := None;
};

let cancel = id => ongoingCallbacks->MapStr.remove(id);

let run = (payload, callback, timeoutCallback) => {
  let messageId = Utils.generateId();

  getWorker()
  ->Toplevel.Top.postMessageToWorker({t_id: messageId, t_message: payload});

  let timeoutId =
    Js.Global.setTimeout(
      () => {
        switch (ongoingCallbacks->MapStr.get(messageId)) {
        | None => ()
        | Some(_) => timeoutCallback()
        };
        terminate(messageId);
      },
      timeoutSeconds^ * 1000,
    );

  ongoingCallbacks->MapStr.set(messageId, (timeoutId, callback));
  messageId;
};

let registerReadyCb = cb =>
  if (isReady^) {
    cb();
  } else {
    readyCb := Some(cb);
  };

let execute = (~lang, ~blocks, ~links, callback) =>
  run(Execute(lang, blocks, links), ExecuteCallback(callback), () =>
    callback(Belt.Result.Error("Evaluation timeout."))
  );

let executeEmbed = (~lang, ~code, callback) =>
  run(ExecuteEmbed(lang, code), ExecuteEmbedCallback(callback), () =>
    callback(Belt.Result.Error("Evaluation timeout."))
  );

let refmt = (refmtTypes, blocks, callback) =>
  run(Refmt(refmtTypes, blocks), RefmtCallback(callback), () =>
    callback(Belt.Result.Error("Evaluation timeout."))
  );

let loadScript = (url, callback) =>
  run(LoadScript(url), LoadScriptCallback(callback), callback);
