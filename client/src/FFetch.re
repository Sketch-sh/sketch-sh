/*
  * Better API design would look like this
  let getRaw = url =>
   Fetch.fetch(url)
   ->FutureJs.fromPromise(_err => Err.NotSent) // we have structured errors representing things that can go wrong
   ->Future.map(Result.flatMap(Err.fromFetch)); // fail on bad status codes here

 let getJson = url =>
   getRaw(url)
   ->Future.flatMapOk(v =>
       Fetch.Response.json(v)->FutureJs.fromPromise(_ => Err.InvalidResponse)
     );

 let getDecoded = (url, decode: Js.Json.t => Belt.Result.t('a, 'e)) =>
   getJson(url)->Future.map(Result.flatMap(Err.fromDecode(decode)));

    type t('decodeErr) =
    | NotSent // failed to begin making request
    | NetworkError(NetworkError.t) // status errors, etc
    | InvalidResponse // we wanted json (probably) and didn't get it
    | DecodeError('decodeErr); // provided decoder couldn't decode the server data
  */
type error = [
  | `NetworkError(string)
  | `ApiErrorJson(string, Js.Json.t)
  | `ApiErrorText(string, string)
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
          : Future.value(Belt.Result.Error(`ApiErrorJson((url, json))))
      );
  };

let text: string => Future.t(Belt.Result.t(string, [> error])) =
  url => {
    fetch(url, Fetch.Response.text)
    ->FutureJs.fromPromise(err => `NetworkError(Js.String.make(err)))
    ->Future.flatMapOk(((isOk, res)) =>
        isOk
          ? Future.value(Belt.Result.Ok(res))
          : Future.value(Belt.Result.Error(`ApiErrorText((url, res))))
      );
  };
