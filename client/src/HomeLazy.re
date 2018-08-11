module Config = {
  module type t = (module type of Home);
};

include ReLoadable.WithRender(Config);
