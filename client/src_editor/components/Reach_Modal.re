module DialogOverlay = {
  [@bs.module "@reach/dialog"] [@react.component]
  external make:
    (
      ~isOpen: bool,
      ~onDismiss: unit => unit,
      ~className: string,
      ~children: React.element
    ) =>
    React.element =
    "DialogOverlay";
};

module DialogContent = {
  [@bs.module "@reach/dialog"] [@react.component]
  external make:
    (~className: string, ~children: React.element) => React.element =
    "DialogContent";
};
