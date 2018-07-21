module S = Terminal_Content_Style;

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

type syntax =
  | Ml
  | Reason;

type line =
  | Welcome(string)
  | Input((string, syntax))
  | ResultOk(string)
  | ResultError(string);

type state = {
  inputValue: string,
  inputDisplay: string,
  history: list((string, syntax)),
  historyCursor: int,
  displayStack: array(line),
  currentSyntax: syntax,
};

type action =
  | InputUpdateValue(string)
  | InputHistory(int)
  | InputEvaluate;

let component = ReasonReact.reducerComponent("Terminal_Content");

let make = _children => {
  ...component,
  initialState: () => {
    inputValue: {|
type say = | Hello | Goodbye;
let say =
fun
  | Hello => ()
  | Goodbye => ()
  | _ => ();
    |},
    inputDisplay: "",
    history: [("", Reason)],
    historyCursor: (-1),
    displayStack: [|Welcome(welcomeText)|],
    currentSyntax: Reason,
  },
  reducer: (action, state) =>
    switch (action) {
    | InputUpdateValue(inputValue) =>
      ReasonReact.Update({...state, inputValue, inputDisplay: inputValue, historyCursor: (-1)})
    | InputHistory(amount) =>
      let cursor = state.historyCursor + amount;
      let length = state.history |. Belt.List.length;

      if (cursor >= length - 1) {
        ReasonReact.NoUpdate;
      } else if (cursor < (-1)) {
        ReasonReact.Update({...state, historyCursor: (-1), inputDisplay: state.inputValue});
      } else {
        switch (state.history |. Belt.List.get(cursor)) {
        | None => ReasonReact.NoUpdate
        | Some((inputDisplay, _syntax)) =>
          ReasonReact.Update({...state, inputDisplay, historyCursor: cursor})
        };
      };
    | InputEvaluate =>
      let inputValue = state.inputValue |. Js.String.trim;

      switch (inputValue, state.currentSyntax) {
      | ("#reset;", Reason)
      | ("#reset;;", Ml) =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            inputValue: "",
            inputDisplay: "",
            history: [(inputValue, state.currentSyntax), ...state.history],
            displayStack:
              Belt.Array.concat(
                [|Input((inputValue, state.currentSyntax))|],
                state.displayStack,
              ),
          },
          (_ => Reason_Evaluator.reset()),
        )
      | ("#toggle_syntax;", Reason) =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            inputValue: "",
            inputDisplay: "",
            currentSyntax: Ml,
            history: [(inputValue, state.currentSyntax), ...state.history],
            displayStack:
              Belt.Array.concat(
                [|Input((inputValue, state.currentSyntax))|],
                state.displayStack,
              ),
          },
          (_ => Reason_Evaluator.mlSyntax()),
        )
      | ("#toggle_syntax;;", Ml) =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            inputValue: "",
            inputDisplay: "",
            currentSyntax: Reason,
            history: [(inputValue, state.currentSyntax), ...state.history],
            displayStack:
              Belt.Array.concat(
                [|Input((inputValue, state.currentSyntax))|],
                state.displayStack,
              ),
          },
          (_ => Reason_Evaluator.mlSyntax()),
        )
      | _ =>
        let result = Reason_Evaluator.execute(inputValue);
        let result =
          switch (result) {
          | Ok(evaluate) => [|ResultOk(evaluate)|]
          | OkWithLog(evaluate, log) => [|ResultOk(evaluate ++ "\n" ++ log)|]
          | OkWithError(evaluate, error) => [|
              ResultOk(evaluate),
              Reason_Evaluator.parseError(~content=inputValue, ~error) |. ResultError,
            |]
          | Error(error) => [|
              Reason_Evaluator.parseError(~content=inputValue, ~error) |. ResultError,
            |]
          | Log(stdout) => [|ResultOk(stdout)|]
          | Nothing => [||]
          };
        ReasonReact.Update({
          ...state,
          inputValue: "",
          inputDisplay: "",
          history: [(inputValue, state.currentSyntax), ...state.history],
          displayStack:
            Belt.Array.concatMany([|
              result,
              [|Input((inputValue, state.currentSyntax))|],
              state.displayStack,
            |]),
        });
      };
    },
  render: ({state, send}) =>
    S.(
      <Fragment>
        <pre>
          <div className=stack>
            (
              state.displayStack
              |. Belt.Array.mapWithIndexU((. index, line) => {
                   let simpleLine = ((className, line)) =>
                     <div ?className key=(index |. string_of_int)> (line |. str) </div>;
                   /* dangerouslySetInnerHTML */
                   switch (line) {
                   | ResultOk(line) => (None, line) |. simpleLine
                   | ResultError(line) => <div dangerouslySetInnerHTML={"__html": line} />
                   | Welcome(line) => (Some(lineWelcome), line) |. simpleLine
                   | Input((line, syntax)) =>
                     switch (syntax) {
                     | Reason => (Some(lineInputRe), line) |. simpleLine
                     | Ml => (Some(lineInputMl), line) |. simpleLine
                     }
                   };
                 })
              |. ReasonReact.array
            )
          </div>
        </pre>
        <div className=inputWrapper>
          <span className=sharpText>
            (
              (
                switch (state.currentSyntax) {
                | Reason => "Reason # "
                | Ml => "Ocaml # "
                }
              )
              |. str
            )
          </span>
          <Textarea
            value=state.inputDisplay
            minRows=10
            className=inputC
            autoFocus=true
            onChange=(event => event |. valueFromEvent |. InputUpdateValue |. send)
            onKeyDown=(
              event => {
                let keyName = event |. ReactEventRe.Keyboard.key;
                let shiftKey = event |. ReactEventRe.Keyboard.shiftKey;

                switch (keyName) {
                | "Enter" =>
                  if (! shiftKey) {
                    let shouldEvaluate =
                      switch (state.currentSyntax) {
                      | Reason => Analyzer.shouldEvaluateRe(state.inputValue)
                      | Ml => Analyzer.shouldEvaluateMl(state.inputValue)
                      };
                    if (shouldEvaluate) {
                      event |. ReactEventRe.Keyboard.preventDefault;
                      send(InputEvaluate);
                    };
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
