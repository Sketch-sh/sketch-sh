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

let displayInput = (syntax, input) =>
  input
  |> splitOnChar('\n')
  |> List.mapi((i, line) =>
       i == 0 ?
         line :
         (
           switch (syntax) {
           | Ml => "<span class=\"dim2\">Ocaml # </span>" ++ line
           | Reason => "<span class=\"dim2\">Reason # </span>" ++ line
           }
         )
     )
  |> String.concat("\n");

type line =
  | Welcome(string)
  | Input((string, syntax))
  | Result(Reason_Evaluator.executeResult);

type state = {
  inputValue: string,
  inputDisplay: string,
  history: list((string, syntax)),
  historyCursor: int,
  displayStack: array(line),
  currentSyntax: syntax,
};

type action =
  | UpdateState(state)
  | CodeEvaluated(string, Reason_Evaluator.executeResult)
  | InputUpdateValue(string)
  | InputHistory(int)
  | InputEnter;

let component = ReasonReact.reducerComponent("Terminal_Content");

let make = _children => {
  ...component,
  initialState: () => {
    inputValue: "",
    inputDisplay: "",
    history: [("", Reason)],
    historyCursor: (-1),
    displayStack: [|Welcome(welcomeText)|],
    currentSyntax: Reason,
  },
  reducer: (action, state) =>
    switch (action) {
    | UpdateState(state) => ReasonReact.Update(state)
    | InputUpdateValue(inputValue) =>
      ReasonReact.Update({
        ...state,
        inputValue,
        inputDisplay: inputValue,
        historyCursor: (-1),
      })
    | InputHistory(amount) =>
      let cursor = state.historyCursor + amount;
      let length = state.history |. Belt.List.length;

      if (cursor >= length - 1) {
        ReasonReact.NoUpdate;
      } else if (cursor < (-1)) {
        ReasonReact.Update({
          ...state,
          historyCursor: (-1),
          inputDisplay: state.inputValue,
        });
      } else {
        switch (state.history |. Belt.List.get(cursor)) {
        | None => ReasonReact.NoUpdate
        | Some((inputDisplay, _syntax)) =>
          ReasonReact.Update({...state, inputDisplay, historyCursor: cursor})
        };
      };
    | InputEnter =>
      let inputValue = state.inputValue |. Js.String.trim;

      switch (inputValue, state.currentSyntax) {
      | ("#reset;", Reason)
      | ("#reset;;", Ml) =>
        ReasonReact.SideEffects(
          (
            ({state, send}) =>
              Js.Promise.(
                Reason_Evaluator.reset(.)
                |> then_(() =>
                     {
                       ...state,
                       inputValue: "",
                       inputDisplay: "",
                       history: [
                         (inputValue, state.currentSyntax),
                         ...state.history,
                       ],
                       displayStack:
                         Belt.Array.concat(
                           state.displayStack,
                           [|Input((inputValue, state.currentSyntax))|],
                         ),
                     }
                     |. UpdateState
                     |. send
                     |. resolve
                   )
                |> handleError
              )
              |> ignore
          ),
        )
      | ("#toggle_syntax;", Reason) =>
        ReasonReact.SideEffects(
          (
            ({state, send}) =>
              Js.Promise.(
                Reason_Evaluator.mlSyntax(.)
                |> then_(() =>
                     {
                       ...state,
                       inputValue: "",
                       inputDisplay: "",
                       currentSyntax: Ml,
                       history: [
                         (inputValue, state.currentSyntax),
                         ...state.history,
                       ],
                       displayStack:
                         Belt.Array.concat(
                           state.displayStack,
                           [|Input((inputValue, state.currentSyntax))|],
                         ),
                     }
                     |. UpdateState
                     |. send
                     |. resolve
                   )
                |> handleError
              )
              |> ignore
          ),
        )
      | ("#toggle_syntax;;", Ml) =>
        ReasonReact.SideEffects(
          (
            ({state, send}) =>
              Js.Promise.(
                Reason_Evaluator.reasonSyntax(.)
                |> then_(() =>
                     {
                       ...state,
                       inputValue: "",
                       inputDisplay: "",
                       currentSyntax: Reason,
                       history: [
                         (inputValue, state.currentSyntax),
                         ...state.history,
                       ],
                       displayStack:
                         Belt.Array.concat(
                           state.displayStack,
                           [|Input((inputValue, state.currentSyntax))|],
                         ),
                     }
                     |. UpdateState
                     |. send
                     |. resolve
                   )
                |> handleError
              )
              |> ignore
          ),
        )
      | _ =>
        ReasonReact.SideEffects(
          (
            ({send}) =>
              Js.Promise.(
                Reason_Evaluator.execute(inputValue)
                |> then_((result: Reason_Evaluator.executeResult) =>
                     switch (result.stderr) {
                     | None =>
                       CodeEvaluated(inputValue, result) |. send |. resolve
                     | Some(stderr) =>
                       Reason_Evaluator.parseError(
                         ~content=inputValue,
                         ~error=stderr,
                       )
                       |> then_(stderr => {
                            let result = {...result, stderr: Some(stderr)};
                            CodeEvaluated(inputValue, result)
                            |. send
                            |. resolve;
                          })
                       |> handleError
                     }
                   )
                |> handleError
              )
              |> ignore
          ),
        )
      };
    | CodeEvaluated(inputValue, result) =>
      ReasonReact.Update({
        ...state,
        inputValue: "",
        inputDisplay: "",
        history: [(inputValue, state.currentSyntax), ...state.history],
        displayStack:
          Belt.Array.concat(
            state.displayStack,
            [|
              Input((
                inputValue |> displayInput(state.currentSyntax),
                state.currentSyntax,
              )),
              Result(result),
            |],
          ),
      })
    },
  render: ({state, send}) =>
    S.(
      <Fragment>
        <pre>
          <div className=stack>
            (
              state.displayStack
              |. Belt.Array.mapWithIndexU((. index, line) => {
                   let key = index |. string_of_int;
                   let simpleLine = ((className, line)) =>
                     <div ?className key> (line |. str) </div>;
                   switch (line) {
                   | Welcome(line) =>
                     (Some(lineWelcome), line) |. simpleLine
                   | Input((line, syntax)) =>
                     switch (syntax) {
                     | Reason =>
                       <div
                         key
                         className=lineInputRe
                         dangerouslySetInnerHTML={"__html": line}
                       />
                     | Ml =>
                       <div
                         key
                         className=lineInputMl
                         dangerouslySetInnerHTML={"__html": line}
                       />
                     }
                   | Result({stderr, stdout, evaluate}) =>
                     <div key>
                       (
                         stderr
                         =>> (
                           stderr =>
                             <div
                               dangerouslySetInnerHTML={"__html": stderr}
                             />
                         )
                       )
                       (stdout =>> (stdout => <div> (stdout |. str) </div>))
                       (
                         evaluate
                         =>> (
                           evaluate =>
                             <div className="cyan"> (evaluate |. str) </div>
                         )
                       )
                     </div>
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
            onChange=(
              event => event |. valueFromEvent |. InputUpdateValue |. send
            )
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
                      send(InputEnter);
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
