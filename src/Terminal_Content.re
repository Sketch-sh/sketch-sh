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
  let lineError = Css.([%style {|
          color: red;
        |}] |. style);
  let lineInput =
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
          font-family: monospace;
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
  | ResultOk(string)
  | ResultError(string);

type state = {
  inputValue: string,
  inputDisplay: string,
  historyCursor: option(int),
  stack: list(line),
};

type action =
  | InputUpdateValue(string)
  | InputHistory(int)
  | InputEvaluate;

let component = ReasonReact.reducerComponent("Terminal_Content");

let make = _children => {
  ...component,
  initialState: () => {
    stack: [Welcome(welcomeText)],
    inputValue: "",
    inputDisplay: "",
    historyCursor: None,
  },
  reducer: (action, state) =>
    switch (action) {
    | InputUpdateValue(inputValue) =>
      ReasonReact.Update({
        ...state,
        inputValue,
        inputDisplay: inputValue,
        historyCursor: None,
      })
    | InputHistory(amount) =>
      let cursor =
        switch (state.historyCursor) {
        | None => amount
        | Some(pos) => pos + 2 * amount
        };

      if (cursor >= (state.stack |. Belt.List.length) - 1) {
        ReasonReact.NoUpdate;
      } else if (cursor < 0) {
        ReasonReact.Update({
          ...state,
          historyCursor: None,
          inputDisplay: state.inputValue,
        });
      } else {
        let inputDisplay =
          switch (state.stack |. Belt.List.get(cursor)) {
          | Some(line) =>
            switch (line) {
            | Input(string) => string
            | _ =>
              raise(Invalid_argument("This shouldn't happen wrong line"))
            }
          | None => raise(Invalid_argument("This shouldn't happen no line"))
          };
        ReasonReact.Update({
          ...state,
          inputDisplay,
          historyCursor: Some(cursor),
        });
      };
    | InputEvaluate =>
      let v = state.inputValue |. Js.String.trim;
      let result = Reason_Evaluator.execute(v);
      let result =
        switch (result) {
        | Belt.Result.Ok(line) => ResultOk(line)
        | Error(line) => ResultError(line)
        };
      ReasonReact.Update({
        ...state,
        inputValue: "",
        inputDisplay: "",
        stack: [result, Input(v), ...state.stack],
      });
    },
  render: ({state, send}) =>
    S.(
      <Fragment>
        <pre>
          <div className=stack>
            (
              state.stack
              |. Belt.List.mapWithIndexU((. index, line) => {
                   let (className, line) =
                     switch (line) {
                     | ResultOk(line) => (None, line)
                     | ResultError(line) => (Some(lineError), line)
                     | Welcome(line) => (Some(lineWelcome), line)
                     | Input(line) => (Some(lineInput), line)
                     };
                   <div ?className key=(index |. string_of_int)>
                     (line |. str)
                   </div>;
                 })
              |. Array.of_list
              |. ReasonReact.array
            )
          </div>
        </pre>
        <div className=inputWrapper>
          <span className=sharpText> ("Reason # " |. str) </span>
          <Textarea
            value=state.inputDisplay
            minRows=10
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
                | "ArrowUp" =>
                  if (! shiftKey) {
                    event |. ReactEventRe.Keyboard.preventDefault;
                    send(InputHistory(1));
                  }
                | "ArrowDown" =>
                  if (! shiftKey) {
                    event |. ReactEventRe.Keyboard.preventDefault;
                    send(InputHistory(-1));
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
