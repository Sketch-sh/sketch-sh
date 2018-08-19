[@bs.module "react-content-loader"]
external reactClass: ReasonReact.reactClass = "default";

[@bs.deriving abstract]
type jsProps = {
  [@bs.optional]
  animate: bool,
  [@bs.optional]
  speed: float,
  [@bs.optional]
  className: string,
  [@bs.optional]
  width: int,
  [@bs.optional]
  height: int,
  [@bs.optional]
  preserveAspectRatio: string,
  [@bs.optional]
  primaryColor: string,
  [@bs.optional]
  secondaryColor: string,
  [@bs.optional]
  primaryOpacity: float,
  [@bs.optional]
  secondaryOpacity: float,
  [@bs.optional]
  style: ReactDOMRe.Style.t,
  [@bs.optional]
  uniquekey: string,
};

let make =
    (
      ~animate=?,
      ~speed=?,
      ~className=?,
      ~width=?,
      ~height=?,
      ~preserveAspectRatio=?,
      ~primaryColor=?,
      ~secondaryColor=?,
      ~primaryOpacity=?,
      ~secondaryOpacity=?,
      ~style=?,
      ~uniquekey=?,
      children,
    ) =>
  ReasonReact.wrapJsForReason(
    ~reactClass,
    ~props=
      jsProps(
        ~animate?,
        ~speed?,
        ~className?,
        ~width?,
        ~height?,
        ~preserveAspectRatio?,
        ~primaryColor?,
        ~secondaryColor?,
        ~primaryOpacity?,
        ~secondaryOpacity?,
        ~style?,
        ~uniquekey?,
        (),
      ),
    children,
  );
