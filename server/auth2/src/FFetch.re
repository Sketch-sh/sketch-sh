/*
  * Better API design would look like this
  let getRaw = url =>
   Fetch.fetch(url)
   ->FutureJs.fromPromise(_err => Err.NotSent) // we have structured errors representing things that can go wrong
   ->Future.map(Result.flatMap(Err.fromFetch)); // fail on bad status codes here

 let getJson = url =>
   getRaw(url)q
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
type network_error = {status: int};

type error =
  | Not_sent
  | Response_error
  | Network_error(network_error)
  | Invalid_repsone;

let fetch = url => {
  Fetch.fetch(url)->FutureJs.fromPromise->Future.flatMapError(a => {});
};
