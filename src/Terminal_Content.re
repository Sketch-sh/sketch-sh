open Utils;

let welcomeText = {|
      ___  _______   ________  _  __
    / _ \/ __/ _ | / __/ __ \/ |/ /
   / , _/ _// __ |_\ \/ /_/ /    /
  /_/|_/___/_/ |_/___/\____/_/|_/

Execute statements/let bindings.
Hit <enter> after the semicolon.

> let myVar = "Hello Reason!";
> let myList: list(string) = ["first", "second"];
|};

module S = {
  let stack =
    Css.(
      [%style
        {|
            display: flex;
            flex-direction: column-reverse;
        |}
      ]
      |. style
    );
  let lineWelcome =
    Css.([%style {|
            text-align: center;
        |}] |. style);
  let sharp =
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
  let sharpText =
    Css.([%style {|
          color: green;
        |}] |. style);
  let inputWrapper =
    Css.([%style {|
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
          unsafe: "border" "none";
          unsafe: "outline" "none";
        |}
      ]
      |. style
    );
};
type line =
  | Welcome(string)
  | Input(string)
  | Result(string);

type state = {
  inputValue: string,
  stack: list(line),
};

type action =
  | InputUpdateValue(string)
  | InputEvaluate;

let component = ReasonReact.reducerComponent("Terminal_Content");

let make = _children => {
  ...component,
  initialState: () => {stack: [Welcome(welcomeText)], inputValue: ""},
  reducer: (action, state) =>
    switch (action) {
    | InputUpdateValue(inputValue) =>
      ReasonReact.Update({...state, inputValue})
    | InputEvaluate =>
      let v = state.inputValue |. Js.String.trim;
      let result = Reason_Evaluator.execute(v);
      let result =
        switch (result) {
        | Belt.Result.Ok(r) => r
        | Error(r) => r
        };

      ReasonReact.Update({
        /* ...state, */
        inputValue: "",
        stack: [Result(result), Input(v), ...state.stack],
      });
    },
  render: ({state, send}) =>
    S.(
      <Fragment>
        <pre>
          <div className=stack>
            (
              state.stack
              |. Belt.List.mapWithIndexU((. index, line) =>
                   switch (line) {
                   | Result(line) =>
                     <div key=(index |. string_of_int)> (line |. str) </div>
                   | Welcome(line) =>
                     <div className=lineWelcome key=(index |. string_of_int)>
                       (line |. str)
                     </div>
                   | Input(line) =>
                     <div className=sharp key=(index |. string_of_int)>
                       (line |. str)
                     </div>
                   }
                 )
              |. Array.of_list
              |. ReasonReact.array
            )
          </div>
        </pre>
        <div className=inputWrapper>
          <span className=sharpText> ("Reason # " |. str) </span>
          <textarea
            value=state.inputValue
            className=inputC
            autoFocus=true
            onChange=(
              event => event |. valueFromEvent |. InputUpdateValue |. send
            )
            onKeyDown=(
              event => {
                let keyName = event |. ReactEventRe.Keyboard.key;
                let shiftKey = event |. ReactEventRe.Keyboard.shiftKey;

                switch (keyName) {
                | "Enter" =>
                  if (! shiftKey && Utils.isClosed(state.inputValue)) {
                    event |. ReactEventRe.Keyboard.preventDefault;
                    send(InputEvaluate);
                  }
                | _ => ()
                };
              }
            )
          />
        </div>
      </Fragment>
    ),
};
