module Config = {
  module type t = (module type of NoteTemplateChoose);
};

include ReLoadable.WithRender(Config);
