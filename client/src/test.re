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
let a = () => {
  let future = Future.value(42);
  let%map new_value = future;
  a => a + 1;

  Js.log(new_value);
};
