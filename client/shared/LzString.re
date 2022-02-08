type t;

let async: unit => Js.Promise.t(t) = () => Modules.require("lz-string");

module URI = {
  [@bs.send]
  external compress: (t, string) => string = "compressToEncodedURIComponent";
  [@bs.send]
  external decompress: (t, string) => string =
    "decompressFromEncodedURIComponent";
};
