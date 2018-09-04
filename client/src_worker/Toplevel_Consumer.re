open Toplevel.Types;

let timeoutSeconds = ref(10);
let toplevelWorker = ref(None);

module MapStr = Belt.MutableMap.String;

type callback =
  | LinkCallback(
      Belt.Result.t(
        list((Editor_Types.Link.link, Worker_Types.linkResult)),
        string,
      ) =>
      unit,
    )
  | ExecuteCallback(Belt.Result.t(list(blockResult), string) => unit)
  | RefmtCallback(Belt.Result.t(refmtOk, string) => unit);

let ongoingCallbacks: MapStr.t((Js.Global.timeoutId, callback)) =
  MapStr.make();

let workerListener = event => {
  let {w_id, w_message: message} = event##data;

  switch (ongoingCallbacks->MapStr.get(w_id)) {
  | None => ()
  | Some((timeoutId, callback)) =>
    Js.Global.clearTimeout(timeoutId);
    switch (message) {
    | Ready => () /* Ignore this for now */
    | LinkResult(result) =>
      switch (callback) {
      | LinkCallback(callback) => callback(Belt.Result.Ok(result))
      | _ => ()
      }
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

let link = (links, callback) =>
  run(Link(links), LinkCallback(callback), () =>
    callback(Belt.Result.Error("Evaluation timeout"))
  );

let execute = (lang, blocks, callback) =>
  run(Execute(lang, blocks), ExecuteCallback(callback), () =>
    callback(Belt.Result.Error("Evaluation timeout"))
  );

let refmt = (refmtTypes, blocks, callback) =>
  run(Refmt(refmtTypes, blocks), RefmtCallback(callback), () =>
    callback(Belt.Result.Error("Evaluation timeout"))
  );
