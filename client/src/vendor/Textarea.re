[@bs.deriving abstract]
type jsProps = {
  value: string,
  [@bs.optional]
  className: string,
  [@bs.optional]
  onChange: ReactEventRe.Form.t => unit,
  [@bs.optional]
  onKeyDown: ReactEventRe.Keyboard.t => unit,
  [@bs.optional]
  autoFocus: bool,
  [@bs.optional]
  minRows: int,
  [@bs.optional]
  maxRows: int,
};

[@bs.module "react-textarea-autosize"]
external myJSReactClass : ReasonReact.reactClass = "default";

let make =
    (
      ~className=?,
      ~value,
      ~onChange=?,
      ~onKeyDown=?,
      ~autoFocus=?,
      ~minRows=?,
      ~maxRows=?,
      children,
    ) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=myJSReactClass,
    ~props=
      jsProps(
        ~value,
        ~className?,
        ~onChange?,
        ~onKeyDown?,
        ~autoFocus?,
        ~minRows?,
        ~maxRows?,
        (),
      ),
    children,
  );
