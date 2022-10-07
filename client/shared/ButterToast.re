module Tray = {
  [@bs.module "butter-toast"] [@react.component]
  external makeRaw:
    (
      ~trayPosition: string=?,
      ~toastMargin: int=?,
      ~renderInContext: bool=?,
      ~name: string=?,
      ~pauseOnHover: bool=?,
      ~theme: string=?
    ) =>
    React.element =
    "default";

  [@bs.module "butter-toast"]
  external reactClass: ReasonReact.reactClass = "default";

  type trayPosition =
    | TopLeft
    | TopCenter
    | TopRight
    | BottomLeft
    | BottomCenter
    | BottomRight;
  let trayPositionToString =
    fun
    | TopLeft => "top-left"
    | TopCenter => "top-center"
    | TopRight => "top-right"
    | BottomLeft => "bottom-left"
    | BottomCenter => "bottom-center"
    | BottomRight => "bottom-right";

  [@react.component]
  let make =
      (
        ~trayPosition: option(trayPosition)=?,
        ~toastMargin=?,
        ~renderInContext=?,
        ~name=?,
        ~pauseOnHover=?,
        ~theme=?,
      ) => {
    let trayPosition = Belt.Option.map(trayPosition, trayPositionToString);
    React.createElement(
      makeRaw,
      makeRawProps(
        ~trayPosition?,
        ~toastMargin?,
        ~renderInContext?,
        ~name?,
        ~pauseOnHover?,
        ~theme?,
        (),
      ),
    );
  };
};

module ToastOption = {
  [@bs.deriving abstract]
  type contentArgs = {
    toastId: string,
    dismiss: unit => unit,
  };

  [@bs.deriving abstract]
  type t = {
    content: contentArgs => React.element,
    [@bs.optional]
    dismissOnClick: bool,
    [@bs.optional]
    sticky: bool,
    [@bs.optional]
    name: string,
    [@bs.optional]
    toastTimeout: int,
  };

  let make =
      (
        ~dismissOnClick=?,
        ~sticky=?,
        ~name=?,
        ~toastTimeout=?,
        content: (~toastId: string, ~dismiss: unit => unit) => React.element,
      ) =>
    t(
      ~content=
        contentArgs =>
          content(
            ~toastId=contentArgs->toastIdGet,
            ~dismiss=contentArgs->dismissGet,
          ),
      ~dismissOnClick?,
      ~sticky?,
      ~name?,
      ~toastTimeout?,
      (),
    );
};

type t;
[@bs.module "butter-toast"] external toast: t = "default";
[@bs.send] external _make: (t, ToastOption.t) => unit = "raise";
let make = _make(toast);
let option = ToastOption.make;
