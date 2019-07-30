module D = Decode.AsResult.OfParseError;

type error = [ FFetch.error | `DecodeError(D.ParseError.failure)];

let json:
  (string, Js.Json.t => Belt.Result.t('decoded, D.ParseError.failure)) =>
  Future.t(Belt.Result.t('decoded, [> error])) =
  (url, decoder) => {
    FFetch.json(url)
    ->Future.flatMapOk(json =>
        decoder(json)
        ->(
            fun
            | Belt.Result.Ok(a) => Belt.Result.Ok(a)
            | Belt.Result.Error(err) => Belt.Result.Error(`DecodeError(err))
          )
        |> Future.value
      );
  };
