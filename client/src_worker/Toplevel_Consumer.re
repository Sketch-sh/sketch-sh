open Toplevel.Types;

let timeoutSeconds = ref(180);
let toplevelWorker406 = ref(None);
let toplevelWorker413 = ref(None);

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

let getWorkerRef = (~compilerVersion) =>
  switch (compilerVersion) {
  | CompilerVersion.V4_06 => toplevelWorker406
  | V4_13_1 => toplevelWorker413
  };

let getWorker = (~compilerVersion) => {
  let pick = worker =>
    switch (worker^) {
    | None =>
      let newWorker = Toplevel.make(~compilerVersion);
      worker := Some(newWorker);
      newWorker->Toplevel.Top.onMessageFromWorker(workerListener);
      newWorker->Toplevel.Top.onErrorFromWorker(Js.log);
      newWorker;
    | Some(worker) => worker
    };
  getWorkerRef(~compilerVersion)->pick;
};
let terminate = (~compilerVersion, id) => {
  ongoingCallbacks->MapStr.remove(id);
  getWorker(~compilerVersion)->Toplevel.Top.terminate;
  getWorkerRef(~compilerVersion) := None;
};

let cancel = id => ongoingCallbacks->MapStr.remove(id);

let run = (~compilerVersion, payload, callback, timeoutCallback) => {
  let messageId = Utils.generateId();

  getWorker(~compilerVersion)
  ->Toplevel.Top.postMessageToWorker({t_id: messageId, t_message: payload});

  let timeoutId =
    Js.Global.setTimeout(
      () => {
        switch (ongoingCallbacks->MapStr.get(messageId)) {
        | None => ()
        | Some(_) => timeoutCallback()
        };
        terminate(~compilerVersion, messageId);
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

let execute = (~compilerVersion, ~lang, ~blocks, ~links, callback) =>
  run(
    ~compilerVersion,
    Execute(lang, blocks, links),
    ExecuteCallback(callback),
    () =>
    callback(Belt.Result.Error("Evaluation timeout."))
  );

let executeEmbed = (~compilerVersion, ~lang, ~code, callback) =>
  run(
    ~compilerVersion,
    ExecuteEmbed(lang, code),
    ExecuteEmbedCallback(callback),
    () =>
    callback(Belt.Result.Error("Evaluation timeout."))
  );

let refmt = (~compilerVersion, refmtTypes, blocks, callback) =>
  run(
    ~compilerVersion, Refmt(refmtTypes, blocks), RefmtCallback(callback), () =>
    callback(Belt.Result.Error("Evaluation timeout."))
  );

let loadScript = (~compilerVersion, url, callback) =>
  run(
    ~compilerVersion,
    LoadScript(url),
    LoadScriptCallback(callback),
    callback,
  );
