let stack =
  Css.(
    [%style {|
        display: flex;
        flex-direction: column-reverse;
        |}] |. style
  );
let lineWelcome = Css.([%style {|
            text-align: center;
        |}] |. style);
let lineError = Css.([%style {|
          color: red;
        |}] |. style);
let lineInputRe =
  Css.(
    [%css
      {|
          ::before {
            color: green;
            unsafe: "content" "Reason #";
            margin-right: 0.5rem;
          }
        |}
    ]
    |. style
  );
let lineInputMl =
  Css.(
    [%css
      {|
          ::before {
            color: green;
            unsafe: "content" "OCaml #";
            margin-right: 0.5rem;
          }
        |}
    ]
    |. style
  );
let sharpText = Css.([%style {|
          color: green;
        |}] |. style);
let inputWrapper = Css.([%style {|
            display: flex;
        |}] |. style);
let inputC =
  Css.(
    [%style
      {|
          background: transparent;
          color: #fff;
          flex: 1;
          height: 100px;
          margin-left: 0.5rem;
          padding: 0;
          font-family: monospace;
          unsafe: "border" "none";
          unsafe: "outline" "none";
        |}
    ]
    |. style
  );
