open Jest;

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

/* open Worker_Types.CompilerErrorMessage; */

describe("parsing error messages from ocaml compiler", () => {
  open Expect;
  open! Expect.Operators;
  test("single error message", () =>
    expect(parse(error)) |> toMatchSnapshot
  );
  /* expect(parse(error))
     == [|
          Err_Warning({
            o_content: "Warning 11: this match case is unused.",
            o_pos: ({o_line: 5, o_col: 2}, {o_line: 5, o_col: 5}),
          }),
        |] */
  test("multi error messages", () =>
    parse(errors) |> expect |> toMatchSnapshot
  );
});
