type t('a);

[@bs.module] external make: Js.Promise.t('a) => t('a) = "make-cancelable";

[@bs.send] external cancel: t('a) => unit = "";
[@bs.send] external catch: (t('a), Js.t({..}) => unit) => t('a) = "";
