module Secure = {
  /* [@bs.module] external make : unit => string = "nanoid"; */
  [@bs.module] external generate: (string, int) => string = "nanoid/generate";
};
