/* open TestFramework;
open Core.Evaluate;

let initialize = () => {
  Toploop.initialize_toplevel_env();
  Toploop.input_name := "//toplevel//";
  Repl.SyntaxControl.re();
};

let eval = Repl.Evaluate.eval(~readStdout=(module ReadStdoutUnix));

let makeLoc = (loc_start, loc_end) => {
  Core.Loc.{
    loc_start: {
      line: fst(loc_start),
      col: snd(loc_start),
    },
    loc_end: {
      line: fst(loc_end),
      col: snd(loc_end),
    },
  };
};

let makeWarning = (~sub=[], ~number, ~msg, block_start, block_end) => {
  {
    warnLoc: Some(makeLoc(block_start, block_end)),
    warnNumber: number,
    warnMsg: msg,
    warnSub: sub,
  };
};

let success = (~warnings=[], ~stdout="", msg, block_start, block_end) =>
  Phrase({
    blockLoc: Some(makeLoc(block_start, block_end)),
    blockContent: BlockSuccess({msg, warnings}),
    blockStdout: stdout,
  });

let error =
    (
      ~warnings=[],
      ~stdout="",
      ~errSub=[],
      msg,
      block_loc,
      error_start,
      error_end,
    ) => {
  Phrase({
    blockLoc:
      block_loc
      |> Util.Option.map(((block_start, block_end)) =>
           makeLoc(block_start, block_end)
         ),
    blockContent:
      BlockError({
        error: {
          errLoc: Some(makeLoc(error_start, error_end)),
          errMsg: msg,
          errSub,
        },
        warnings,
      }),
    blockStdout: stdout,
  });
};

describe("success test", ({test, _}) => {
  test("single line, multiple phrases", ({expect}) => {
    initialize();

    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "let x = 1; let y = 2; let z = 3;",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(3);
    let calls = Mock.getCalls(mock) |> List.rev;

    expect.equal(
      List.nth(calls, 0),
      success("let x: int = 1;", (0, 0), (0, 8)),
    );
    expect.equal(
      List.nth(calls, 1),
      success("let y: int = 2;", (0, 11), (0, 19)),
    );
    expect.equal(
      List.nth(calls, 2),
      success("let z: int = 3;", (0, 22), (0, 30)),
    );
  });

  test("multiple lines, multiple phrases", ({expect}) => {
    initialize();

    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "let x = 1;\nlet y = 2;\nlet z = 3;",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(3);
    let calls = Mock.getCalls(mock) |> List.rev;

    expect.equal(
      List.nth(calls, 0),
      success("let x: int = 1;", (0, 0), (0, 8)),
    );
    expect.equal(
      List.nth(calls, 1),
      success("let y: int = 2;", (1, 0), (1, 8)),
    );
    expect.equal(
      List.nth(calls, 2),
      success("let z: int = 3;", (2, 0), (2, 8)),
    );
  });

  test("single phrases in multiple lines", ({expect}) => {
    initialize();
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "let myFunc = () => {\n  1\n}",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(1);
    let calls = Mock.getCalls(mock) |> List.rev;

    expect.equal(
      List.nth(calls, 0),
      success("let myFunc: unit => int = <fun>;", (0, 0), (2, 0)),
    );
  });

  test("with warnings", ({expect}) => {
    initialize();
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "type warn = | Foo | Bar;\nfun | Foo => () | Bar => () | _ => ();",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(2);
    let calls = Mock.getCalls(mock) |> List.rev;

    expect.equal(
      List.nth(calls, 1),
      success(
        ~warnings=[
          makeWarning(
            ~number=11,
            ~msg="this match case is unused.",
            (1, 28),
            (1, 30),
          ),
        ],
        "- : warn => unit = <fun>",
        (1, 0),
        (1, 36),
      ),
    );
  });
});

describe("error tests", ({test, _}) => {
  test("syntax error", ({expect}) => {
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(~send=Mock.fn(mock), ~complete=Mock.fn(mockComplete), "let a = {");
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalError);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(1);
    expect.mock(mock).toBeCalledWith(
      error(
        ~errSub=[
          (Some(makeLoc((0, 8), (0, 8))), "This '{' might be unmatched"),
        ],
        "Syntax error: '}' expected",
        None,
        (0, 8),
        (0, 8),
      ),
    );
  });

  test("single line, error as last phrase", ({expect}) => {
    initialize();

    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "let a = 1; let b = \"2\"; a + b;",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalError);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(3);

    let calls = Mock.getCalls(mock) |> List.rev;

    expect.equal(
      List.nth(calls, 0),
      success("let a: int = 1;", (0, 0), (0, 8)),
    );

    expect.equal(
      List.nth(calls, 1),
      success("let b: string = \"2\";", (0, 11), (0, 21)),
    );

    expect.equal(
      List.nth(calls, 2),
      error(
        "This expression has type string but an expression was expected of type\n         int",
        Some(((0, 24), (0, 28))),
        (0, 28),
        (0, 28),
      ),
    );
  });
});

describe("stdout", ({test, _}) =>
  test("redirect stdout", ({expect}) => {
    initialize();
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "print_endline(\"Hello world\")",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(1);
    expect.mock(mock).toBeCalledWith(
      success(~stdout="Hello world\n", "- : unit = ()", (0, 0), (0, 27)),
    );
  })
);

describe("directives", ({test, _}) => {
  test("directive ouutput to stdout", ({expect}) => {
    initialize();
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "let a = 1;\n#show_val a;",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(2);
    expect.mock(mock).toBeCalledWith(Directive("let a: int;\n"));
  });

  test("directive output to Toploop.execute_phrase buffer", ({expect}) => {
    initialize();
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "#show_val 1;",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledWith(
      Directive("Wrong type of argument for directive `show_val'.\n"),
    );
  });

  test("directive with error", ({expect}) => {
    initialize();
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "#show_val a;",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */

    expect.mock(mock).toBeCalledWith(Directive("Unbound value a"));
  });

  test(
    "ignore directives with error and continue the evaluation", ({expect}) => {
    initialize();
    let mock = Mock.mock1(_ => ());
    let mockComplete = Mock.mock1(_ => ());

    eval(
      ~send=Mock.fn(mock),
      ~complete=Mock.fn(mockComplete),
      "#show_val a; let x = 1; let y = 2;",
    );
    /* Inspect overal result */
    expect.mock(mockComplete).toBeCalledTimes(1);
    expect.mock(mockComplete).toBeCalledWith(EvalSuccess);
    /* Inspect each block calls */
    expect.mock(mock).toBeCalledTimes(3);
    expect.mock(mock).toBeCalledWith(Directive("Unbound value a"));
    let calls = Mock.getCalls(mock) |> List.rev;
    expect.equal(
      List.nth(calls, 1),
      success("let x: int = 1;", (0, 13), (0, 21)),
    );
  });
}); */
