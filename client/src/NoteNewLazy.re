module Config = {
  module type t = (module type of NoteNew);
};

module Loadable = ReLoadable.WithRender(Config);

let make = (~title=?, ~blocks=?, ~lang, ~links=?, _children) =>
  Loadable.make(
    ~fetch=() => DynamicImport.import("./NoteNew.bs.js"),
    ~onLoading=() => <UI_FullpageLoading.Jsx2 />,
    ~render=((module NoteNew)) => <NoteNew ?title ?blocks ?links lang />,
    [||],
  );
