// open SStdlib;

// let handlers = ref([]);

// let to_string = error => {
//   let rec loop =
//     fun
//     | [] => None
//     | [handler, ...xs] =>
//       handler(error)->Option.flatMapNone(() => loop(xs));

//   loop(handlers^)->Option.getWithDefault("Unknow how to print this error");
// };
