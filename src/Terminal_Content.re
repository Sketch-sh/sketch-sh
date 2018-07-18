open Utils;

let welcomeText = {|
                 ___  _______   ________  _  __
                / _ \/ __/ _ | / __/ __ \/ |/ /
               / , _/ _// __ |_\ \/ /_/ /    /
              /_/|_/___/_/ |_/___/\____/_/|_/

Execute statements/let bindings. Hit <enter> after the semicolon. Ctrl-d to quit.

      >   let myVar = "Hello Reason!";
      >   let myList: list(string) = ["first", "second"];
|};

module S = {
  let sharp =
    Css.(
      [%css
        {|
          ::before {
            unsafe: "content" "Reason #";
            margin-right: 0.5rem;
          }
        |}
      ]
      |. style
    );
  let inputC =
    Css.(
      [%style
        {|
          background: transparent;
          unsafe: "border" "none";
          color: #fff;
          unsafe: "outline" "none";
        |}
      ]
      |. style);
};
type state = {stack: list(string)};

type action =
  | Add(string);

let component = ReasonReact.reducerComponent("Terminal_Content");

let make = _children => {
  ...component,
  initialState: () => {stack: [welcomeText]},
  reducer: (action, state) =>
    switch (action) {
    | Add(line) =>
      ReasonReact.Update({...state, stack: [line, ...state.stack]})
    },
  render: ({state}) =>
    S.(
      <Fragment>
        <pre>
          <div>
            (
              state.stack
              |. Belt.List.mapWithIndexU((. index, line) =>
                   if (index == 0) {
                     <div key=(index |. string_of_int)> (line |. str) </div>;
                   } else {
                     <div className=sharp key=(index |. string_of_int)>
                       (line |. str)
                     </div>;
                   }
                 )
              |. Array.of_list
              |. ReasonReact.array
            )
          </div>
        </pre>
        <div className=sharp> 
          <input className=inputC autoFocus=true />
        </div>
      </Fragment>
    ),
};
