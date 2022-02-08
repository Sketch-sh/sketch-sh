Modules.require("./Balloon.css");

type position =
  | Up
  | Left
  | Right
  | Down;

let positionToString =
  fun
  | Up => "up"
  | Left => "left"
  | Right => "right"
  | Down => "down";

type length =
  | Small
  | Medium
  | Large
  | Fit;

let lengthToString =
  fun
  | Small => "small"
  | Medium => "medium"
  | Large => "large"
  | Fit => "fit";

[@react.component]
let make = (~message, ~position, ~visible=?, ~length=?, ~children) => {
  React.cloneElement(
    children,
    {
      "message": message,
      "position": position->positionToString,
      "visible": visible,
      "length": length->Belt.Option.map(lengthToString),
    },
  );
};
