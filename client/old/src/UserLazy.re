module Config = {
  module type t = (module type of User);
};

include ReLoadable.WithRender(Config);
