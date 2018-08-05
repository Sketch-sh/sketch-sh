module Secure = {
  [@bs.module] external make : unit => string = "nanoid";
  [@bs.module "nanoid"]
  external generate : (string, int) => string = "generate";
};
