let let_ =
    (promise: Js.Promise.t('a), continue: 'a => Js.Promise.t('b))
    : Js.Promise.t('b) =>
  promise |> Js.Promise.then_(x => continue(x));
