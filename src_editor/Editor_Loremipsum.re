let code1 = {|print_endline("Hello ReasonML folks!");

let pi = 4. *. atan(1.);

let random_gaussian = () =>
  1.
  +. sqrt((-2.) *. log(Random.float(1.)))
  *. cos(2. *. pi *. Random.float(1.));

for (i in 0 to 40) {
  random_gaussian() |> string_of_float |> print_endline;
};|};

let code2 = {|type tree = Leaf | Node(int, tree, tree);

let rec sum = (item) => {
  switch (item) {
  | Leaf => 0
  | Node(value, left, right) => value + sum(left) + sum(right);
  }
};

let myTree =
  Node(
    1,
    Node(2, Node(4, Leaf, Leaf), Node(6, Leaf, Leaf)),
    Node(3, Node(5, Leaf, Leaf), Node(7, Leaf, Leaf))
  );

Printf.sprintf("%i", sum(myTree));|};
