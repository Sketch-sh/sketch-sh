[@bs.module "react-helmet"] [@react.component]
external make:
  (
    ~encodeSpecialCharacters: bool=?,
    ~titleTemplate: string=?,
    ~defaultTitle: string=?,
    ~children: React.element=?
  ) =>
  React.element =
  "default";
