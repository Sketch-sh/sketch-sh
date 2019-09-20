[@bs.deriving abstract]
type authenticate_callback_options = {
  session: bool,
  failureRedirect: string,
};

type strategy;

[@bs.module "passport"] external use: strategy => unit = "use";

// [@bs.module "passport"]
// external authenticate: string => Express.Middleware.t = "authenticate";

module Github = {
  [@bs.deriving abstract]
  type options = {
    clientID: string,
    clientSecret: string,
    callbackURL: string,
    scope: array(string),
  };

  type callback_done('error, 'ok) =
    (. Js.Nullable.t('error), Js.Nullable.t('ok)) => unit;
  type callback('error, 'ok) =
    (
      . string,
      Js.Nullable.t(string),
      Js.Json.t,
      callback_done('error, 'ok)
    ) =>
    unit;

  [@bs.new] [@bs.module "passport-github2"]
  external make: (options, callback('error, 'ok)) => strategy = "Strategy";

  [@bs.module "passport"]
  external authenticate: ([@bs.as "github"] _) => Express.Middleware.t =
    "authenticate";

  [@bs.module "passport"]
  external authenticate_callback:
    ([@bs.as "github"] _, authenticate_callback_options) =>
    Express.Middleware.t =
    "authenticate";
};
