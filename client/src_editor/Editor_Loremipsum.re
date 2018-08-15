let code1 = {|/* Inline evaluation and stdout */
print_endline("Hello ReasonML folks!");
type say = | Hello | Bye;

/* A warning */
fun
| Hello
| Bye => ()
| _ => ();

/* A error */
a|};

let code3 = {|type say = | Hello | Bye;
fun
| Hello => ()
| Bye => ()
| _ => ();|};

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
