module E = Express;

let router: E.Router.t = E.Router.make();

[@bs.module] external helmet: unit => Express.Middleware.t = "helmet";
router->E.Router.use(helmet());

Endpoints.apply(router);

let app = E.App.make();
app->ExpressExt.appSet("trust proxy", true);
app->Express.App.use(
  CookieSession.middleware({
    "maxAge": Some(1000 * 60 * 60 * 24 * 7),
    "domain": Config.isProduction ? Some("dayone.app") : None,
    "secure": Some(Config.isDev ? false : true),
    "secret": Config.jwtSecret,
  }),
);

if (Config.isDev) {
  app->E.App.use(ExpressLog.middleware);
};

E.App.useRouter(app, router);

app->E.App.use(
  E.Static.make("static", E.Static.defaultOptions())->E.Static.asMiddleware,
);


let port = Config.port;

E.App.listen(
  app,
  ~port,
  ~onListen=_ => Log.info2("Listening on port", port),
  (),
);

// schedule stuff
Schedulers.init();