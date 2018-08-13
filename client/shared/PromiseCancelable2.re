type t('a) = unit => unit;

[@bs.module] external make: Js.Promise.t('a) => t('a) = "make-cancelable";
