open Express;

[@decco]
type query = {redirectUrl: string};

let handler_auth_github =
  Middleware.from((next, req, res) => {
    let params = req |> Request.query |> Json.object_;
    switch (query_decode(params)) {
    | Error(e) =>
      res
      |> Response.status(Response.StatusCode.BadGateway)
      |> Response.sendString(
           "Could not decode expected params from query string: location="
           ++ e.path
           ++ ", message="
           ++ e.message,
         )
    | Ok({redirectUrl}) =>
      let host =
        Node_url.make(redirectUrl)
        ->Option.flatMap(({Node_url.host}) => host)
        ->Option.map(host => Config.login_redirects_domain->Arr.has(host));

      Log.debug2(
        "Verify redirectUrl",
        {
          "redirectUrl": redirectUrl,
          "host": host,
          "allowedDomains": Config.login_redirects_domain,
        },
      );

      switch (host) {
      | Some(true) =>
        let _ =
          Response.cookie(
            ~name="auth_github_redirect_to",
            ~httpOnly=true,
            Js.Json.string(redirectUrl),
            res,
          );
        next(Next.middleware, res);
      | _ =>
        res
        |> Response.status(Response.StatusCode.BadRequest)
        |> Response.sendString("Invalid or unauthorzied redirectUrl")
      };
    };
  });
