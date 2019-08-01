open Pom;

module Status = {
  include Express.Response.StatusCode;
};

type res =
  | BadRequest(string)
  | NotFound(string)
  | Unauthorized(string)
  | OkString(string)
  | OkJson(Js.Json.t)
  | StatusString(Status.t, string)
  | StatusJson(Status.t, Js.Json.t)
  | TemporaryRedirect(string)
  | RespondRaw(Express.Response.t => Express.complete);

exception HttpException(res);

let abort = res => {
  raise(HttpException(res));
};

type verb =
  | GET
  | POST
  | PUT
  | DELETE;

type guard('a) = Express.Request.t => pom('a);

let requireBody: Decco.decoder('body) => guard('body) =
  (decoder, req) => {
    // We resolve a promise first so that if the decoding
    // fails, it will reject the promise instead of just
    // throwing and requiring a try/catch.
    let%Pom _ = resolved();
    switch (req |> Express.Request.bodyJSON) {
    | Some(rawBodyJson) =>
      switch (decoder(rawBodyJson)) {
      | Error(e) =>
        raise(
          HttpException(
            BadRequest(
              "Could not decode expected body: location="
              ++ e.path
              ++ ", message="
              ++ e.message,
            ),
          ),
        )
      | Ok(v) => v->resolved
      }
    | None => abort @@ BadRequest("Body required")
    };
  };

let requireParams: Decco.decoder('params) => guard('params) =
  (decoder, req) => {
    let paramsAsJson: Js.Json.t = Obj.magic(req |> Express.Request.params);
    // We resolve a promise first so that if the decoding
    // fails, it will reject the promise instead of just
    // throwing and requiring a try/catch.
    let%Pom _ = resolved();
    switch (decoder(paramsAsJson)) {
    | Result.Error(e) =>
      abort @@
      BadRequest(
        "Could not decode expected params from the URL path: location="
        ++ e.path
        ++ ", message="
        ++ e.message,
      )
    | Result.Ok(v) => resolved @@ v
    };
  };

let requireQuery: Decco.decoder('query) => guard('query) =
  (decoder, req) => {
    // We resolve a promise first so that if the decoding
    // fails, it will reject the promise instead of just
    // throwing and requiring a try/catch.
    let%Pom _ = resolved();
    switch (decoder(ExpressExt.queryJson(req))) {
    | Result.Error(e) =>
      abort @@
      BadRequest(
        "Could not decode expected params from query string: location="
        ++ e.path
        ++ ", message="
        ++ e.message,
      )
    | Result.Ok(v) => resolved @@ v
    };
  };

type handler = Express.Request.t => pom(res);

module type HandlerConfigWithCustomMiddleware = {
  let path: string;
  let verb: verb;
  let middleware: option(array(Express.Middleware.t));
  let handler: handler;
};

module MakeWithCustomMiddleware = (Cfg: HandlerConfigWithCustomMiddleware) => {
  let use = (router: Express.Router.t) => {
    let verbFunction =
      switch (Cfg.verb) {
      | GET => Express.Router.getWithMany
      | POST => Express.Router.postWithMany
      | PUT => Express.Router.putWithMany
      | DELETE => Express.Router.deleteWithMany
      };

    let resToExpressRes = (res, handlerRes) =>
      switch (handlerRes) {
      | BadRequest(msg) =>
        Express.Response.(
          res |> status(Status.BadRequest) |> sendString(msg)
        )
      | NotFound(msg) =>
        Express.Response.(res |> status(Status.NotFound) |> sendString(msg))
      | Unauthorized(msg) =>
        Express.Response.(
          res |> status(Status.Unauthorized) |> sendString(msg)
        )
      | OkString(msg) =>
        Express.Response.(
          res
          |> status(Status.Ok)
          |> setHeader("content-type", "text/plain; charset=utf-8")
          |> sendString(msg)
        )
      | OkJson(js) =>
        Express.Response.(res |> status(Status.Ok) |> sendJson(js))
      | StatusString(stat, msg) =>
        Express.Response.(
          res
          |> status(stat)
          |> setHeader("content-type", "text/plain; charset=utf-8")
          |> sendString(msg)
        )
      | StatusJson(stat, js) =>
        Express.Response.(res |> status(stat) |> sendJson(js))
      | TemporaryRedirect(location) =>
        Express.Response.(
          res
          |> setHeader("Location", location)
          |> sendStatus(StatusCode.TemporaryRedirect)
        )
      | RespondRaw(fn) => fn(res)
      };

    let wrappedHandler = (_next, req, res) => {
      let handleOCamlError =
        [@bs.open]
        (
          fun
          | HttpException(handlerResponse) =>
            resToExpressRes(res, handlerResponse)
        );
      let handleError = error => {
        switch (handleOCamlError(error)) {
        | Some(complete) => complete->resolved
        | None =>
          switch (Obj.magic(error)##stack) {
          | Some(stack) =>
            Log.error2("Unhandled internal server error", stack)
          | None => Log.error2("Unhandled internal server error", error)
          };
          Express.Response.(res |> sendStatus(Status.InternalServerError))
          ->resolved;
        };
      };

      switch (Cfg.handler(req)) {
      | exception err => handleError(err)->toJsPromise
      | p => p->map(resToExpressRes(res))->catch(handleError)->toJsPromise
      };
    };

    let expressHandler = Express.PromiseMiddleware.from(wrappedHandler);

    router->verbFunction(
      ~path=Cfg.path,
      Array.concat(
        Cfg.middleware->Option.getWithDefault([||]),
        [|RequestContext.contextMiddleware, expressHandler|],
      ),
    );
  };
};

module type HandlerConfig = {
  let path: string;
  let verb: verb;
  let handler: handler;
};

module Make = (Cfg: HandlerConfig) => {
  module NewCfg = {
    let path = Cfg.path;
    let verb = Cfg.verb;
    let handler = Cfg.handler;
    let middleware =
      Some([|
        // By default we parse JSON bodies
        Express.Middleware.json(~limit=Express.ByteLimit.Mb(10.), ()),
      |]);
  };
  module A = MakeWithCustomMiddleware(NewCfg);
  include A;
};

// Modules and functors for use with let_anything:
module type HandlerOptConfig = {let response: res;};
module FailHandlerIfNone = (Cfg: HandlerOptConfig) => {
  let let_ = (option, after) => {
    switch (option) {
    | Some(v) => after(v)
    | None => abort(Cfg.response)
    };
  };
};