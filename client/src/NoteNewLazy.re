module Config = {
  module type t = (module type of NoteNew);
};

module Loadable = ReLoadable.WithRender(Config);

[@react.component]
let make = (~title=?, ~blocks=?, ~lang, ~links=?) =>
  <Loadable
    fetch={() => DynamicImport.import("./NoteNew.bs.js")}
    onLoading={() => <UI_FullpageLoading />}
    render={((module NoteNew)) => <NoteNew ?title ?blocks ?links lang />}
  />;
