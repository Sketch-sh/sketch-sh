module Config = {
  module type t = (module type of NoteNew);
};

module Loadable = ReLoadable.WithRender(Config);

let make = (~title=?, ~blocks=?, _children) =>
  Loadable.make(
    ~fetch=
      () => [%bs.raw
        {| import(/* webpackChunkName: "NoteNew" */ "./NoteNew.bs.js") |}
      ],
    ~onLoading=() => <UI_FullpageLoading />,
    ~render=((module NoteNew)) => <NoteNew ?title ?blocks />,
    [||],
  );
