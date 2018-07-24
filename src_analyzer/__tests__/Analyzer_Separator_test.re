open Jest;

let code = {|
let a = 1;

let b = 2;
|};

let code2 = {|
let zero = (f, x) => x;

let succ = (n, f, x) => f(n(f, x));

let one = succ(zero);

let two = succ(succ(zero));

let add = (n1, n2, f, x) => n1(f, n2(f, x));

let to_string = n => n(k => "S" ++ k, "0");

to_string(add(succ(two), two));
|};

let code3 = {|let a=1;let b=2;|};

let code4 = [|
  {|let exprSeperator = code => {
  let length = String.length(code);

  let exprs = ref([]);

  let bracketState = ref(BracketCounting.make());

  let rec exprSeperator' = (i, line, col, exprStartOffset, shouldCount) =>
    if (i < length) {
      switch (code.[i]) {
      | '\n' =>
        exprSeperator'(
          i + 1,
          line + 1,
          0,
          shouldCount ? exprStartOffset : exprStartOffset + 1,
          shouldCount,
        )
      | ' ' =>
        exprSeperator'(
          i + 1,
          line,
          col + 1,
          shouldCount ? exprStartOffset : exprStartOffset + 1,
          shouldCount,
        )
      | char =>
        if (shouldCount && bracketState^(char)) {
          bracketState := BracketCounting.make();
          exprs :=
            [
              String.sub(code, exprStartOffset, i - exprStartOffset + 1),
              ...exprs^,
            ];
          exprSeperator'(i + 1, line, col + 1, i + 1, false);
        } else {
          exprSeperator'(i + 1, line, col + 1, exprStartOffset, true);
        }
      };
    } else {
      exprs^ |. List.rev;
    };
  exprSeperator'(0, 0, 0, 0, false);
};|},
  "let foo=1;",
  "let bar=2;",
|];

let code5 = {|module BracketCounting = {
  type t = {
    roundo: int,
    roundc: int,
    curlyo: int,
    curlyc: int,
    squareo: int,
    squarec: int,
    quote: int,
  };

  let initialState = {
    roundo: 0,
    roundc: 0,
    curlyo: 0,
    curlyc: 0,
    squareo: 0,
    squarec: 0,
    quote: 0,
  };

  let make = () => {
    let bracketState = ref(initialState);

    let loop = char => {
      let s = bracketState^;
      switch (char) {
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
      | '"' =>
        bracketState := {...s, quote: s.quote + 1};
        false;
      | ';' =>
        if (s.roundo == s.roundc
        && s.curlyo == s.curlyc
        && s.squareo == s.squarec
        && s.quote
        land 1 == 0) {
          bracketState := initialState;
          true
        }
        else {
          false
        }
      | _ => false
      };
    };
    loop;
  };
};|};

let make = Analyzer_Separator.exprSeperator;

external _stringify : array(string) => Js.Json.t = "%identity";

Skip.describe(
  "Separate code block into expressions",
  () => {
    open Expect;
    open! Expect.Operators;

    test("simple", () =>
      expect(make(code) |> Array.of_list)
      == [|"let a = 1;", "let b = 2;"|]
    );
    test("complex", () =>
      expect(make(code2) |> Array.of_list)
      == [|
           "let zero = (f, x) => x;",
           "let succ = (n, f, x) => f(n(f, x));",
           "let one = succ(zero);",
           "let two = succ(succ(zero));",
           "let add = (n1, n2, f, x) => n1(f, n2(f, x));",
           {|let to_string = n => n(k => "S" ++ k, "0");|},
           "to_string(add(succ(two), two));",
         |]
    );
    test("bad indentation", () =>
      expect(make(code3) |> Array.of_list) == [|"let a=1;", "let b=2;"|]
    );
    test("long function body", () =>
      expect(make(code4 |> Js.Array.joinWith("\n")) |> Array.of_list)
      == code4
    );
    test("nested module", () =>
      expect(make(code5) |> List.length) == 5
    );
  },
);
