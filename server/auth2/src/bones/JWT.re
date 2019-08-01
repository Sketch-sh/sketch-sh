module Sign_options = {
  type t;
  [@bs.obj]
  external make:
    (
      ~expiresIn: string=?,
      ~algorithm: [@bs.string] [ | `RS256 | `RS512]=?,
      unit
    ) =>
    t =
    "";
};

[@bs.module "jsonwebtoken"]
external verify: (~token: string, ~secret: string) => Js.Json.t = "verify";

[@bs.module "jsonwebtoken"]
external sign:
  (~payload: Js.Json.t, ~secret: string, ~options: Sign_options.t) => string =
  "sign";
