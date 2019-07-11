module Config = {
  module type t = (module type of NoteNew);
};

module Loadable = ReLoadable.WithRender(Config);

let make = (~title=?, ~blocks=?, ~lang, ~links=?, _children) =>
  Loadable.make(
    ~fetch=
      () =>
        %bs.raw
        {| import(/* webpackChunkName: "NoteNew" */ "./NoteNew.bs.js") |},
    ~onLoading=() => <UI_FullpageLoading />,
    ~render=((module NoteNew)) => <NoteNew ?title ?blocks lang ?links />,
    [||],
  );
