[@bs.deriving abstract]
type jsProps = {
  [@bs.optional]
  encodeSpecialCharacters: bool,
  [@bs.optional]
  titleTemplate: string,
  [@bs.optional]
  defaultTitle: string,
};

[@bs.module "react-helmet"]
external reactClass: ReasonReact.reactClass = "default";

let make =
    (~encodeSpecialCharacters=?, ~titleTemplate=?, ~defaultTitle=?, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass,
    ~props=
      jsProps(~encodeSpecialCharacters?, ~titleTemplate?, ~defaultTitle?, ()),
    children,
  );
