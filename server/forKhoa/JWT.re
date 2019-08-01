type options = {. "expiresIn": string};
type error = {
  .
  "name": string,
  "message": string,
};

[@bs.module "jsonwebtoken"]
external verify: (~token: string, ~secret: string) => Js.Json.t = "";

[@bs.module "jsonwebtoken"]
external sign:
  (~payload: Js.Json.t, ~secret: string, ~options: options) => string =
  "";