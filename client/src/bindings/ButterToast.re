module Tray = {
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

  [@bs.deriving abstract]
  type jsProps = {
    [@bs.optional]
    trayPosition: string,
    [@bs.optional]
    toastMargin: int,
    [@bs.optional]
    renderInContext: bool,
    [@bs.optional]
    name: string,
    [@bs.optional]
    pauseOnHover: bool,
    [@bs.optional]
    theme: string,
  };

  let make =
      (
        ~trayPosition: option(trayPosition)=?,
        ~toastMargin=?,
        ~renderInContext=?,
        ~name=?,
        ~pauseOnHover=?,
        ~theme=?,
        children,
      ) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props=
        jsProps(
          ~trayPosition=?Belt.Option.map(trayPosition, trayPositionToString),
          ~toastMargin?,
          ~renderInContext?,
          ~name?,
          ~pauseOnHover?,
          ~theme?,
          (),
        ),
      children,
    );
};

module ToastOption = {
  [@bs.deriving abstract]
  type contentArgs = {
    toastId: string,
    dismiss: unit => unit,
  };

  [@bs.deriving abstract]
  type t = {
    content: contentArgs => ReasonReact.reactElement,
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
        content:
          (~toastId: string, ~dismiss: unit => unit) =>
          ReasonReact.reactElement,
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
