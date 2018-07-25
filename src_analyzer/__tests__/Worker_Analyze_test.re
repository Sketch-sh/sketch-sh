open Jest;
open Worker_Types;

module Analyze = Worker_Analyze.Make(Worker_NodeEvaluator);

let tap = a => {
  Js.log(a);
  a;
};

let exe = code => Analyze.execute(. code) |> Array.of_list;

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

describe(
  "valid",
  () => {
    open Expect;
    open! Expect.Operators;

    test("1 line", () =>
      expect(exe("let a = 1;")) |> toMatchSnapshot
    );

    test("multiple directive in same line", () =>
      expect(exe("let a = 1; let b = a + a;")) |> toMatchSnapshot
    );

    test("multiple line", () =>
      expect(
        exe(
          {|type tree = Leaf | Node(int, tree, tree);

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

Printf.sprintf("%i", sum(myTree));|},
        ),
      )
      |> toMatchSnapshot
    );
    test(
      "sneaky module contains 1 directive",
      () => {
        let result = exe(sneakyModule);
        expect((result |> Array.length, result[0].executeResult.stderr))
        == (1, None);
      },
    );
  },
);
