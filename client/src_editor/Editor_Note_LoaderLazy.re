module Config = {
  module type t = (module type of Editor_Note_Loader);
};

include ReLoadable.WithRender(Config);
