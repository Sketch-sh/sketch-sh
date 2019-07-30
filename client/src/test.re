// module Let_syntax = {
//   let bind = (m: option('a), f: 'a => option('b)): option('b) =>
//     switch (m) {
//     | None => None
//     | Some(a) => f(a)
//     };
// };

// let a = () => {
//   let%bind number = Some(42);
//   None;
// };
// module Let_syntax = {
//   let bind = (m: option('a), f: 'a => option('b)): option('b) =>
//     switch (m) {
//     | None => None
//     | Some(a) => f(a)
//     };
// };

// let a = () => {
//   let%bind number = Some(42);
//   None;
// };

module Let_syntax = {
  let map = (value, ~f) => {
    Future.map(value, f);
  };
};

module Error = {
  module Let_syntax = {
    let map = (value, ~f) => {
      Future.mapError(value, f);
    };
  };
};

let a = () => {
  let future = Future.value(42);
  let%map new_value = future;
  Js.log2("It worked!", new_value);

  let%map.Error new_value = future;
  Js.log2("uh on", err);
};

module FlatMapOk = {
  let let_:
    (
      Future.t(Belt.Result.t('ok, 'error)),
      'ok => Belt.Result.t('nextok, 'error)
    ) =>
    Future.t(Belt.Result.t('nextok, 'error)) =
    (future, continue) => {
      Future.flatMapOk(future, value => continue(value)->Future.value);
    };
};

let a = () => {
  let future = Future.value(Belt.Result.Ok("1"));
  let%FlatMapOk new_future = future;
  Belt.Result.Ok("2");
};
