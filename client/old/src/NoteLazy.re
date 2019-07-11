module Config = {
  module type t = (module type of Note);
};

include ReLoadable.WithRender(Config);
