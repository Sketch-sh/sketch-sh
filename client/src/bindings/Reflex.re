module Container = {
  [@bs.module "react-reflex"] [@react.component]
  external make:
    (
      ~windowResizeAware: bool=?,
      ~orientation: string=?,
      ~maxRecDepth: int=?,
      ~className: string=?,
      ~style: ReactDOMRe.Style.t=?,
      ~children: React.element=?
    ) =>
    React.element =
    "ReflexContainer";
};
module Element = {
  [@bs.module "react-reflex"] [@react.component]
  external make:
    (
      ~renderOnResizeRate: int=?,
      ~propagateDimensions: bool=?,
      ~resizeHeight: bool=?,
      ~resizeWidth: bool=?,
      ~className: string=?,
      ~size: int=?,
      ~minSize: int=?,
      ~maxSize: int=?,
      ~flex: float=?,
      ~children: React.element=?
    ) =>
    React.element =
    "ReflexElement";
};

module Splitter = {
  [@bs.module "react-reflex"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element=?) => React.element =
    "ReflexSplitter";
};
