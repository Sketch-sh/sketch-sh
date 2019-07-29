type error = [
  | `NetworkError(string)
  | `ApiErrorJson(Js.Json.t)
  | `ApiErrorText(string)
];

let fetch = (url, decoder) => {
  let%Async res = Fetch.fetch(url);
  let isOk = res->Fetch.Response.ok;
  let%Async res = decoder(res);

  Js.Promise.resolve((isOk, res));
};

let json: string => Future.t(Belt.Result.t(Js.Json.t, [> error])) =
  url => {
    fetch(url, Fetch.Response.json)
    ->FutureJs.fromPromise(err => `NetworkError(Js.String.make(err)))
    ->Future.flatMapOk(((isOk, json)) =>
        isOk
          ? Future.value(Belt.Result.Ok(json))
          : Future.value(Belt.Result.Error(`ApiErrorJson(json)))
      );
  };

let text: string => Future.t(Belt.Result.t(string, [> error])) =
  url => {
    fetch(url, Fetch.Response.text)
    ->FutureJs.fromPromise(err => `NetworkError(Js.String.make(err)))
    ->Future.flatMapOk(((isOk, res)) =>
        isOk
          ? Future.value(Belt.Result.Ok(res))
          : Future.value(Belt.Result.Error(`ApiErrorText(res)))
      );
  };
