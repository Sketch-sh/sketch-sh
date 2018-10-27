module DialogOverlay = {
  [@bs.module "@reach/dialog"]
  external reactClass: ReasonReact.reactClass = "DialogOverlay";

  [@bs.deriving abstract]
  type jsProps = {
    isOpen: bool,
    onDismiss: unit => unit,
    className: string,
  };

  let make = (~isOpen, ~onDismiss, ~className, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props=jsProps(~isOpen, ~onDismiss, ~className),
      children,
    );
};

module DialogContent = {
  [@bs.module "@reach/dialog"]
  external reactClass: ReasonReact.reactClass = "DialogContent";

  [@bs.deriving abstract]
  type jsProps = {className: string};

  let make = (~className, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props=jsProps(~className),
      children,
    );
};
