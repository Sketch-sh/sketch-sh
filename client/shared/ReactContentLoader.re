module Default = {
  [@react.component "react-content-loader"] [@bs.module]
  external make:
    (
      ~animate: bool=?,
      ~speed: float=?,
      ~className: string=?,
      ~width: int=?,
      ~height: int=?,
      ~preserveAspectRatio: string=?,
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
};

module Code = {
  [@react.component "react-content-loader"] [@bs.module]
  external make:
    (
      ~animate: bool=?,
      ~speed: float=?,
      ~className: string=?,
      ~width: int=?,
      ~height: int=?,
      ~preserveAspectRatio: string=?,
      ~primaryColor: string=?,
      ~secondaryColor: string=?,
      ~primaryOpacity: float=?,
      ~secondaryOpacity: float=?,
      ~style: ReactDOMRe.Style.t=?,
      ~uniquekey: string=?,
      ~children: React.element=?
    ) =>
    React.element =
    "Code";
};

type contentType =
  | Raw
  | Code;

[@react.component]
let make =
    (
      ~typ=Raw,
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
      ~children=ReactOld.null,
    ) =>
  switch (typ) {
  | Raw =>
    <Default
      ?animate
      ?speed
      ?className
      ?width
      ?height
      ?preserveAspectRatio
      ?primaryColor
      ?secondaryColor
      ?primaryOpacity
      ?secondaryOpacity
      ?style
      ?uniquekey>
      children
    </Default>
  | Code =>
    <Code
      ?animate
      ?speed
      ?className
      ?width
      ?height
      ?preserveAspectRatio
      ?primaryColor
      ?secondaryColor
      ?primaryOpacity
      ?secondaryOpacity
      ?style
      ?uniquekey>
      children
    </Code>
  };
