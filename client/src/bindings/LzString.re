type t;

let async: unit => Js.Promise.t(t) = () => Modules.import("lz-string");

module URI = {
  [@bs.module "lz-string"]
  external compress: string => string = "compressToEncodedURIComponent";
  [@bs.module "lz-string"]
  external decompress: string => string = "decompressFromEncodedURIComponent";
};
