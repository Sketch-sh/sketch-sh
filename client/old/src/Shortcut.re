open Utils;

module ShortcutMap = Belt.Map.String;

type handler = Mousetrap.handler;

type unsubscribeCallback = unit => unit;

type subscribeFun = (~global: bool=?, string, Mousetrap.handler, unit) => unit;

module Store = {
  type shortcutMap = ref(ShortcutMap.t(handler));
  let m = ref(ShortcutMap.empty);

  let globalCallbacks: Js.Dict.t(bool) = Js.Dict.empty();

  /* Mousetrap setup */
  let mousetrap = Mousetrap.make();
  let originalStopCallback = mousetrap##stopCallback;

  let newStopCallback = oldStopCallback =>
    (. e, element, combo) =>
      if ([%bs.raw "this.paused"]) {
        true;
      } else {
        switch (globalCallbacks->Js.Dict.get(combo)) {
        | Some(_) => false
        | None => oldStopCallback(. e, element, combo)
        };
      };
  mousetrap##stopCallback #= newStopCallback(originalStopCallback);

  let bindGlobal = (mousetrap, key, handler) => {
    globalCallbacks->Js.Dict.set(key, true);
    mousetrap->Mousetrap.bind(key, handler);
  };

  let unsubscribe = (key, handler) => {
    let warnUnbinding = () =>
      warn(
        "You're trying to unbind a shortcut that doesn't belong to this callback\n"
        ++ "This might happens because someone else overide your shortcut",
      );
    switch ((m^)->ShortcutMap.get(key)) {
    | Some(cb) =>
      if (cb === handler) {
        m := (m^)->ShortcutMap.remove(key);
        mousetrap->Mousetrap.unbind(key);
      } else {
        warnUnbinding();
      }
    | None => warnUnbinding()
    };
  };

  let subscribe: subscribeFun =
    (~global=false, key, handler) => {
      if ((m^)->ShortcutMap.has(key)) {
        warn("Overiding " ++ key ++ " shortcut");
      };
      let bindMethod = global ? bindGlobal : Mousetrap.bind;
      mousetrap->(bindMethod(key, handler));
      m := (m^)->ShortcutMap.set(key, handler);

      () => unsubscribe(key, handler);
    };
};

module Consumer = {
  let component = ReasonReact.statelessComponent("Shortcut.Consumer");

  [@react.component]
  let make = (~children) => {
    children(Store.subscribe);
  };
};
