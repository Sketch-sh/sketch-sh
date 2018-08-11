module Config = {
  module type t = (module type of NoteNew);
};

include ReLoadable.WithRender(Config);
