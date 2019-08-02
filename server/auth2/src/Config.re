let env = Env.getWithDefault(~key="NODE_ENV", ~default="development");

let is_prod = env == "production";
let is_dev = env == "development";

let port = {
  Env.get("PORT")
  ->Belt.Option.flatMap(port =>
      try (Some(port->int_of_string)) {
      | _exn => None
      }
    )
  ->Belt.Option.getWithDefault(3001);
};

let jwt_secret = Env.getExn("JWT_SECRET");

let login_redirects_domain = {
  Env.getExn("ALLOWED_LOGIN_REDIRECT_DOMAINS")->String.split(",");
};

let github_callback_url = Env.getExn("GITHUB_CALLBACK_URL");
let github_client_id = Env.getExn("GITHUB_CLIENT_ID");
let github_client_secret = Env.getExn("GITHUB_CLIENT_SECRET");
