open Express;

let router: Router.t = Router.make();

[@bs.module] external helmet: unit => Express.Middleware.t = "helmet";
router->Router.use(helmet());

router->Router.get(
  ~path="/health",
  Middleware.from((_next, _req, res) =>
    res
    |> Response.status(Response.StatusCode.BadGateway)
    |> Response.sendString("ok")
  ),
);

let github_auth_options =
  Passport.Github.options(
    ~clientID=Config.github_client_id,
    ~clientSecret=Config.github_client_secret,
    ~callbackURL=Config.github_callback_url,
    ~scope=[|"user:email"|],
  );

Passport.use(
  Passport.Github.make(
    github_auth_options,
    Endpoint_auth_github_callback.github_stragegy_callback,
  ),
);

router->Router.useOnPath(
  ~path="/auth/github/go",
  Endpoint_auth_github.handler_auth_github,
);

router->Router.get(~path="/auth/github/go", Passport.Github.authenticate);

// TODO: handle custom callback
// http://www.passportjs.org/docs/authenticate/#custom-callback

router->Router.getWithMany(
  ~path="/auth/github/callback",
  [|
    Passport.Github.authenticate_callback(
      Passport.authenticate_callback_options(
        ~session=false,
        ~failureRedirect="http://localhost:3000",
      ),
    ),
    Middleware.from((next, req, res) => {
      let user: string = [%bs.raw {|req.user|}];
      Js.log(user);
      res |> Response.sendString("done");
    }),
  |],
);

let app = App.make();
App.useRouter(app, router);
