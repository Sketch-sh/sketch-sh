open Jest;
open Worker_Types;
let error = {|
File "", line 5, characters 2-5:
Warning 11: this match case is unused.


|};

let errors = {|
File "", line 5, characters 2-5:
Warning 11: this match case is unused.
File "", line 3, characters 0-3:
Error: Syntax error
File "", line 3, characters 15-17:
Error: Unbound value pi
File "", line 3, characters 2-17:
Error: Unbound value random_gaussian
|};

let parse = Worker_ParseLocation.parse;

describe(
  "valid",
  () => {
    open Expect;
    open! Expect.Operators;

    test("single", () =>
      parse(error) |> expect |> toMatchSnapshot
    );
    test("multi", () =>
      parse(errors) |> expect |> toMatchSnapshot
    );
  },
);
