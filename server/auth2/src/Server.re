open Express;

let router: Router.t = Router.make();

[@bs.module] external helmet: unit => Express.Middleware.t = "helmet";
router->Router.use(helmet());

Endpoint_health.Endpoint.use(router);

router->Router.get(
  ~path="/health",
  Middleware.from((_next, _req, res) =>
    res
    |> Response.status(Response.StatusCode.BadGateway)
    |> Response.sendString("ok")
  ),
);

router->Router.useOnPath(
  ~path="/auth/github",
  Endpoint_auth_github.handler_auth_github,
);

let app = App.make();
App.useRouter(app, router);
