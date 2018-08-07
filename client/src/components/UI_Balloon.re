let component = ReasonReact.statelessComponent("UI_Balloon");

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

[@bs.deriving abstract]
type balloon = {
  [@bs.as "data-balloon"]
  message: string,
  [@bs.as "data-balloon-pos"]
  position: string,
  [@bs.optional] [@bs.as "data-balloon-visible"]
  visible: bool,
  [@bs.optional] [@bs.as "data-balloon-length"]
  length: string,
};

external hackType: balloon => Js.t('a) = "%identity";

let make = (~message, ~position, ~visible=?, ~length=?, children) => {
  ...component,
  render: _self =>
    ReasonReact.cloneElement(
      children,
      ~props=
        balloon(
          ~message,
          ~position=positionToString(position),
          ~visible?,
          ~length=?Belt.Option.map(length, a => a->lengthToString),
          (),
        )
        ->hackType,
      [||],
    ),
};
