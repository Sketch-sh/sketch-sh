type importable('a);

[@bs.val] external import: string => Js.Promise.t(importable('a)) = "import";

let (<$>): (Js.Promise.t('a), 'a => 'b) => Js.Promise.t('b);

let (<$!>): (Js.Promise.t('a), Js.Promise.error => 'a) => Js.Promise.t('a);

let (>>=): (Js.Promise.t('a), 'a => Js.Promise.t('b)) => Js.Promise.t('b);

let (>>=!):
  (Js.Promise.t('a), Js.Promise.error => Js.Promise.t('a)) =>
  Js.Promise.t('a);

let (=<<): ('a => Js.Promise.t('b), Js.Promise.t('a)) => Js.Promise.t('b);

let (!=<<):
  (Js.Promise.error => Js.Promise.t('a), Js.Promise.t('a)) =>
  Js.Promise.t('a);

let resolve: Js.Promise.t(importable('a)) => Js.Promise.t('a);
