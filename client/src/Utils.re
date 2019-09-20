[@bs.val] external warn: 'a => unit = "console.warn";
[@bs.val] external warn2: ('a, 'b) => unit = "console.warn";
[@bs.val] external logError: 'a => unit = "console.error";
[@bs.val] external logError2: ('a, 'b) => unit = "console.error";

let str = ReasonReact.string;
let optionStr =
  fun
  | None => ReasonReact.null
  | Some(s) => ReasonReact.string(s);

let optionToEmptyString =
  fun
  | None => ""
  | Some(s) => s;
/* Render first element of a result array */
let arrayFirst = (array, ~empty, ~render) =>
  switch (Array.length(array)) {
  | 0 => empty
  | 1 => render(array[0])
  | _ =>
    warn2("This array contains more than 1 element", array);
    render(array[0]);
  };

/*
 Tap log: Log and return the value
 Extremely useful when piping
 */
let tapLog = a => {
  Js.log(a);
  a;
};

let valueFromEvent = (event): string => event->ReactEvent.Form.target##value;

let (=>>) = (value, render) =>
  switch (value) {
  | None => ReasonReact.null
  | Some(value) => render(value)
  };

let handleError = Js.Promise.(catch(error => Js.log(error)->resolve));

let js_countLine = s => s |> Js.String.split("\n") |> Js.Array.length;
let countLine = js_countLine;

/* let countLine = s => {
     let length = String.length(s);

     let rec countLine' = (i, result) =>
       if (i < length) {
         switch (s.[i]) {
         | '\n' => countLine'(i + 1, result + 1)
         | _ => countLine'(i + 1, result)
         };
       } else {
         result;
       };
     countLine'(0, 0);
   }; */

let min = (min, value) => value < min ? min : value;

[@bs.val] external env: string = "process.env.NODE_ENV";

let arrayFindIndex = (array, f) => {
  let length = Array.length(array);
  let rec loop = i =>
    if (i < length) {
      let element = array[i];
      if (f(element)) {
        Some(i);
      } else {
        loop(i + 1);
      };
    } else {
      None;
    };
  loop(0);
};

let arrayFind = (array, f) => {
  let length = Array.length(array);
  let rec loop = i =>
    if (i < length) {
      let element = array[i];
      if (f(element)) {
        Some(array[i]);
      } else {
        loop(i + 1);
      };
    } else {
      None;
    };
  loop(0);
};

external toNullable: string => Js.Nullable.t(string) = "%identity";

let array_has = (arr, value) => arr |> Js.Array.indexOf(value) != (-1);

let joinWithComma =
  Belt.Array.reduce(_, "", (str, acc) =>
    if (str === "") {
      acc;
    } else {
      acc ++ ", " ++ str;
    }
  );

module Object = {
  [@bs.val]
  external assign3: (Js.t({..}), Js.t({..}), Js.t({..})) => Js.t({..}) =
    "Object.assign";
  let merge = (o1: Js.t({..}), o2: Js.t({..})) =>
    assign3(Js.Obj.empty(), o1, o2);
};
