type presets =
  | Combined
  | Common
  | Dev
  | Short
  | Tiny
  | Custom(string);

let presetsToString =
  fun
  | Combined => "combined"
  | Common => "common"
  | Dev => "dev"
  | Short => "short"
  | Tiny => "tiny"
  | Custom(format) => format;

[@bs.module] external make : string => Express.Middleware.t = "morgan";

let make = presets => make(presetsToString(presets));
