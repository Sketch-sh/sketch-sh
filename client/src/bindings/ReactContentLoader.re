[@bs.module "react-content-loader"]
external rawClass: React.element = "default";

[@bs.module "react-content-loader"] external codeClass: React.element = "Code";

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

type contentType =
  | Raw
  | Code;

let contentTypeToClass =
  fun
  | Raw => rawClass
  | Code => codeClass;

[@bs.module "react-content-loader"] [@react.component]
external make:
  (
    ~typ: contentType=?,
    ~animate: bool=?,
    ~speed: float=?,
    ~className: string=?,
    ~width: int=?,
    ~height: int=?,
    ~preserveAspectRatio: bool=?,
    ~primaryColor: string=?,
    ~secondaryColor: string=?,
    ~primaryOpacity: float=?,
    ~secondaryOpacity: float=?,
    ~style: ReactDOMRe.Style.t=?,
    ~uniquekey: string=?,
    ~children: React.element=?
  ) =>
  React.element =
  "default";
