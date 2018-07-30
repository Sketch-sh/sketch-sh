open Jest;
open Worker_Types;

module Analyze = Worker_Analyze.Make(Worker_NodeEvaluator);

let tap = a => {
  Js.log(a);
  a;
};

let exe = code =>
  Analyze.parseCommand(~f=Analyze.tryExecute, code) |> Array.of_list;

let sneakyModule = {|module BracketCounting = {
  type t = {
    roundo: int,
    roundc: int,
    curlyo: int,
    curlyc: int,
    squareo: int,
    squarec: int,
    doubleQuote: int,
    quote: int,
  };

  let initialState = {
    roundo: 0,
    roundc: 0,
    curlyo: 0,
    curlyc: 0,
    squareo: 0,
    squarec: 0,
    doubleQuote: 0,
    quote: 0,
  };

  type stringTyp =
    | Single
    | Double
    | Block;

  let make = () => {
    let bracketState = ref(initialState);
    let prevChar = ref(None);
    let parsingStr: ref(option(stringTyp)) = ref(None);

    let loop = (char) => {
      let s = bracketState^;
      let shouldTerminal = switch (char) {
      | '|' when prevChar^ == Some('{') && parsingStr^ == None  =>
        parsingStr := Some(Block);
        false;
      | '}' when prevChar^ == Some('|') =>
        parsingStr := None;
        false;
      | '"' =>
        if (parsingStr^ == None) {
          parsingStr := Some(Double);
          ();
        };
        bracketState := {...s, doubleQuote: s.doubleQuote + 1};
        false;
      /* Normal brackets */
      | '(' =>
        bracketState := {...s, roundo: s.roundo + 1};
        false;
      | ')' =>
        bracketState := {...s, roundc: s.roundc + 1};
        false;
      | '{' =>
        bracketState := {...s, curlyo: s.curlyo + 1};
        false;
      | '}' =>
        bracketState := {...s, curlyc: s.curlyc + 1};
        false;
      | '[' =>
        bracketState := {...s, squareo: s.squareo + 1};
        false;
      | ']' =>
        bracketState := {...s, squarec: s.squarec + 1};
        false;
      | ';' =>
        if (s.roundo == s.roundc
        && s.curlyo == s.curlyc
        && s.squareo == s.squarec
        && s.quote land 1 == 0
        && s.doubleQuote land 1 == 0
        ) {
          bracketState := initialState;
          true
        }
        else {
          false
        }
      | _ => false
      };
      prevChar := Some(char);
      shouldTerminal
    };
    loop;
  };
};|};

let loc_show = ({line, col}) => Printf.sprintf("(%i, %i)", line, col);

let pos_show = ((from, to_)) =>
  Printf.sprintf("%s - %s", loc_show(from), loc_show(to_));

describe("valid", () => {
  open Expect;
  open! Expect.Operators;

  Only.describe("1 block, 1 line", () => {
    let code = "let a = 1;";
    let result = exe(code)[0];

    test("buffer", () =>
      expect(result.buffer) == code
    );
    test("location ", () =>
      expect(pos_show(result.pos))
      == pos_show(({line: 0, col: 0}, {line: 0, col: 10}))
    );
  });

  describe("multi blocks, 1 line", () => {
    let code = "let a = 1; let b = a + a;";
    let result = exe(code);
    test("buffer", () =>
      expect(result |. Belt.Array.map(block => block.buffer))
      == [|"let a = 1;", "let b = a + a;"|]
    );
    test("location ", () =>
      expect(result |. Belt.Array.map(block => pos_show(block.pos)))
      == [|
           pos_show(({line: 0, col: 0}, {line: 0, col: 10})),
           pos_show(({line: 0, col: 11}, {line: 0, col: 25})),
         |]
    );
  });

  describe("multi blocks, 2 lines", () => {
    let code = "let a = 1;\nlet b = a + a;";

    let result = exe(code);
    test("buffer", () =>
      expect(result |. Belt.Array.map(block => block.buffer))
      == [|"let a = 1;", "let b = a + a;"|]
    );
    test("location ", () =>
      expect(result |. Belt.Array.map(block => pos_show(block.pos)))
      == [|
           pos_show(({line: 0, col: 0}, {line: 0, col: 10})),
           pos_show(({line: 1, col: 0}, {line: 1, col: 14})),
         |]
    );
  });

  describe("multi blocks, 3 lines", () => {
    let code = {|let a = 1;
let b = a + 1;
print_endline("awesome");|};

    let result = exe(code);
    test("buffer", () =>
      expect(result |. Belt.Array.map(block => block.buffer))
      == [|"let a = 1;", "let b = a + 1;", {|print_endline("awesome");|}|]
    );
    test("location ", () =>
      expect(result |. Belt.Array.map(block => pos_show(block.pos)))
      == [|
           pos_show(({line: 0, col: 0}, {line: 0, col: 10})),
           pos_show(({line: 1, col: 0}, {line: 1, col: 14})),
           pos_show(({line: 2, col: 0}, {line: 2, col: 25})),
         |]
    );
  });
  test("sneaky module contains 1 directive", () => {
    let result = exe(sneakyModule);
    expect((result |> Array.length, result[0].executeResult.stderr))
    == (1, None);
  });
  describe("real world", () => {
    let code = {|type tree = Leaf | Node(int, tree, tree);

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
    let result = exe(code);

    test("buffer", () =>
      expect(result |. Belt.Array.map(block => block.buffer))
      == [|
           "type tree = Leaf | Node(int, tree, tree);",
           "let rec sum = (item) => {\n  switch (item) {\n  | Leaf => 0\n  | Node(value, left, right) => value + sum(left) + sum(right);\n  }\n};",
           "let myTree =\n  Node(\n  1,\n  Node(2, Node(4, Leaf, Leaf), Node(6, Leaf, Leaf)),\n  Node(3, Node(5, Leaf, Leaf), Node(7, Leaf, Leaf))\n);",
           {|Printf.sprintf("%i", sum(myTree));|},
         |]
    );
    test("location ", () =>
      expect(result |. Belt.Array.map(block => pos_show(block.pos)))
      == [|
           pos_show(({line: 0, col: 0}, {line: 0, col: 41})),
           pos_show(({line: 2, col: 0}, {line: 7, col: 2})),
           pos_show(({line: 9, col: 0}, {line: 14, col: 2})),
           pos_show(({line: 16, col: 0}, {line: 16, col: 34})),
         |]
    );
  });
});

describe("syntax error", () => {
  open Expect;
  open! Expect.Operators;

  test("error in same line with another block", () => {
    let code = "let a = 1;b";
    let result = exe(code);

    expect(result) |> toMatchSnapshot;
  });

  test("error spawning 2 line", () => {
    let code = "let a = 1;b\ncd";
    let result = exe(code);

    expect(result) |> toMatchSnapshot;
  });
});
