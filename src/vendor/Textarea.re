[@bs.deriving abstract]
type jsProps = {
  [@bs.optional]
  className: string,
  [@bs.optional]
  value: string,
  [@bs.optional]
  onChange: ReactEventRe.Form.t => unit,
  [@bs.optional]
  onKeyDown: ReactEventRe.Keyboard.t => unit,
  [@bs.optional]
  autoFocus: bool,
};

[@bs.module "react-textarea-autosize"]
external myJSReactClass : ReasonReact.reactClass = "default";

let make =
    (
      ~className=?,
      ~value=?,
      ~onChange=?,
      ~onKeyDown=?,
      ~autoFocus=?,
      children,
    ) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=myJSReactClass,
    ~props=
      jsProps(~className?, ~value?, ~onChange?, ~onKeyDown?, ~autoFocus?, ()),
    children,
  );
