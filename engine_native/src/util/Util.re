let (>>) = (f, g, x) => g(f(x));

module Option = {
  let flatMap = f =>
    fun
    | None => None
    | Some(a) => f(a);

  let map = f =>
    fun
    | None => None
    | Some(a) => Some(f(a));
  let getWithDefault = v =>
    fun
    | None => v
    | Some(v) => v;
};

module UList = {
  let chop: (int, list('a)) => list('a) =
    (num, l) => {
      let rec loop = (i, result, aux) =>
        if (i < num) {
          switch (aux) {
          | [] => result
          | [hd, ...aux] => loop(i + 1, [hd, ...result], aux)
          };
        } else {
          result;
        };
      loop(0, [], l) |> List.rev;
    };
};
