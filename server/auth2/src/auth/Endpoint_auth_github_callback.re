let future_to_callback:
  (
    Future.t(result('ok, 'error)),
    Passport.Github.callback_done('error, 'ok)
  ) =>
  unit =
  (future, cb) => {
    future->Future.get(
      fun
      | Error(error) => cb(. Js.Nullable.return(error), Js.Nullable.null)
      | Ok(ok) => cb(. Js.Nullable.null, Js.Nullable.return(ok)),
    );
  };

/**
 * Callback from passport-github
 * Flow:
 * Check for user_identity.type = github with profile.id
 * If user exists, return user.id
 * If not, extract data from profile, create:
 *   - New user
 *   - New user_identity
 * Return user.id
 */
module Github_profile = Model.Github_profile;

let github_stragegy_callback: Passport.Github.callback('error, 'ok) =
  (. accessToken, refreshToken, profile, cb) => {
    let parsed_profile = Github_profile.decoder(profile);

    Future.value(parsed_profile)
    ->Future.flatMapOk(
        ({Github_profile.github_id, username, name, email, avatar}) =>
        {}
      );
    // Log.info3(accessToken, refreshToken, parsed_profile);
    cb(. Js.Nullable.null, "hey");
  };
