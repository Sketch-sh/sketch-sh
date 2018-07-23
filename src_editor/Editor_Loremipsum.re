let code1 = {|let pi = 4. *. (atan 1.)
let random_gaussian () =
  1. +.
    ((sqrt ((-2.) *. (log (Random.float 1.)))) *.
       (cos ((2. *. pi) *. (Random.float 1.))))

let _ = for i = 0 to 40 do
  (random_gaussian ())
  |> string_of_float
  |> print_endline
done|};

let code2 = {|module CodeBlock = {
  [@bs.module "./CodeBlock.js"]
  external reactClass : ReasonReact.reactClass = "default";

  [@bs.deriving abstract]
  type jsProps = {
    value: string,
    onChange: string => unit,
    [@bs.optional]
    firstLineNumber: int,
  };

  let make = (~value, ~onChange, ~firstLineNumber=?, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props=jsProps(~value, ~onChange, ~firstLineNumber?, ()),
      children,
    );
};|};
