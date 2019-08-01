include Decco;

module Legacy = {
  let first = (decoder: Json.Decode.decoder('a), json: Js.Json.t): 'a => {
    open Json.Decode;
    let decoded = json |> list(decoder);
    switch (decoded->List.get(0)) {
    | Some(item) => item
    | None =>
      raise(
        Json.Decode.DecodeError(
          "Expected at least one element in list but got" ++ Obj.magic(json),
        ),
      )
    };
  };

  // Meant specifically for query strings. Allows the user to pass in
  // booleans like this: ?foo | ?foo=1 | ?foo=true | ?foo=false | ?foo=0 etc...
  let boolFromQuery = json => {
    switch (Json.Decode.string(json)) {
    | "" => true
    | "1" => true
    | "true" => true
    | "yes" => true
    | "0" => false
    | "false" => false
    | _ => false
    };
  };

  // Meant specifically for query strings. Allows the user to pass in
  // strings like so: ?excludeJournals=c1g132g,c73aw93,d902ufa
  let stringArrayFromQuery = json => {
    Json.Decode.string(json) |> Js.String.split(",");
  };

  let base64 = (json: Js.Json.t): Node.Buffer.t => {
    json
    |> Json.Decode.string
    |> Node.Buffer.fromStringWithEncoding(_, `base64);
  };

  let buffer = (json: Js.Json.t): Node.Buffer.t => {
    let isBuffer: bool = [%bs.raw "json instanceof Buffer"];
    isBuffer
      ? Obj.magic(json)
      : raise(
          Json.Decode.DecodeError(
            "Expected a buffer, but got " ++ Obj.magic(json),
          ),
        );
  };

  // Returns a Some('a) if the json is a non-empty array.
  // Fails if the first item of the array is not 'a.
  // Returns a None if the array is empty.
  //
  // This is distinct from Json.Decode.optional, which returns
  // None if anything fails at all.
  let maybeFirst = (decoder, json) =>
    Json.Decode.(
      json
      |> (
        array(a => a)
        |> map(arr =>
             switch (arr[0]) {
             | Some(j) => Some(decoder(j))
             | None => None
             }
           )
      )
    );

  /* This is a placeholder in case we need to change how the parsing works for all
     the dates at once. */
  let bigint = (json: Js.Json.t): float =>
    Json.Decode.oneOf(
      [Json.Decode.float, Json.Decode.(map(float_of_string, string))],
      json,
    );
};

let unsafeLegacy = (decoder: Json.Decode.decoder('a), json: Js.Json.t): 'a => {
  switch (decoder(json)) {
  | exception (Json.Decode.DecodeError(err)) =>
    Js.Exn.raiseError({j|Decoder failed $err|j})
  | v => v
  };
};
let unsafe = (decoder: Decco.decoder('a), json: Js.Json.t): 'a => {
  switch (decoder(json)) {
  | Result.Ok(v) => v
  | Result.Error(e) =>
    let path = e.path;
    let message = e.message;
    Js.Exn.raiseError(
      {j|Decoder failed at path $path with message $message|j},
    );
  };
};

let errorToString = (e: Decco.decodeError): string => {
  let path = e.path;
  let message = e.message;
  let value = e.value;
  {j|decoder failed at path $path with message $message on value $value|j};
};

let decodePom =
    (pom: Pom.pom(Js.Json.t), decoder: Decco.decoder('a)): Pom.pom('a) => {
  pom->Pom.flatMap(json =>
    switch (decoder(json)) {
    | Result.Error(e) =>
      Js.Exn.raiseError(
        "Decoding failed: "
        ++ Js.Json.stringifyAny(e)
           ->Option.getWithDefault("no encodable error"),
      )
    | Result.Ok(v) => v->Pom.resolved
    }
  );
};

let decodePomLegacy =
    (pom: Pom.pom(Js.Json.t), decoder: Json.Decode.decoder('a))
    : Pom.pom('a) => {
  pom->Pom.flatMap(json =>
    switch (decoder(json)) {
    | exception (Json.Decode.DecodeError(e)) =>
      Js.Exn.raiseError(
        "Decoding failed: "
        ++ Js.Json.stringifyAny(e)
           ->Option.getWithDefault("no encodable error"),
      )
    | v => v->Pom.resolved
    }
  );
};

let fromLegacy = (decoder: Json.Decode.decoder('a)): Decco.decoder('a) => {
  json => {
    switch (decoder(json)) {
    | exception (Json.Decode.DecodeError(e)) =>
      Result.Error({path: "not available", message: e, value: json})
    | v => Result.Ok(v)
    };
  };
};

module StringArrayFromQuery = {
  type _t = array(string);
  let encoder: Decco.encoder(_t) =
    arr => Decco.arrayToJson(Decco.stringToJson, arr);

  let decoder: Decco.decoder(_t) =
    json => {
      fromLegacy(Legacy.stringArrayFromQuery, json);
    };

  let codec: Decco.codec(_t) = (encoder, decoder);

  [@decco]
  type t = [@decco.codec codec] _t;
};

module BoolFromQuery = {
  let encoder: Decco.encoder(bool) = Decco.boolToJson;

  let decoder: Decco.decoder(bool) = Legacy.boolFromQuery->fromLegacy;

  let codec: Decco.codec(bool) = (encoder, decoder);

  [@decco]
  type t = [@decco.codec codec] bool;
};

module Date = {
  let encoder: Decco.encoder(Js.Date.t) =
    date => Js.Date.toISOString(date)->Decco.stringToJson;

  let decoder: Decco.decoder(Js.Date.t) =
    json => {
      switch (Decco.stringFromJson(json)) {
      | Result.Ok(v) => Js.Date.fromString(v)->Ok
      | Result.Error(_) as err => err
      };
    };

  let codec: Decco.codec(Js.Date.t) = (encoder, decoder);

  [@decco]
  type t = [@decco.codec codec] Js.Date.t;
};

module DateAsFloat = {
  let encoder: Decco.encoder(Js.Date.t) =
    date => Js.Date.valueOf(date)->Json.Encode.float;

  let decoder: Decco.decoder(Js.Date.t) =
    json => {
      switch (Decco.floatFromJson(json)) {
      | Result.Ok(v) => Js.Date.fromFloat(v)->Ok
      | Result.Error(_) as err => err
      };
    };

  let codec: Decco.codec(Js.Date.t) = (encoder, decoder);

  [@decco]
  type t = [@decco.codec codec] Js.Date.t;
};

module NonEmptyListOfString = {
  type _t = NonEmptyList.t(string);

  let encoder: Decco.encoder(_t) =
    lst => Decco.listToJson(Decco.stringToJson, lst->NonEmptyList.toT);

  let decoder: Decco.decoder(_t) =
    json => {
      switch (Decco.listFromJson(Decco.stringFromJson, json)) {
      | Result.Ok(v) =>
        switch (NonEmptyList.fromT(v)) {
        | Some(vv) => Ok(vv)
        | None =>
          Result.Error({
            path: "decoding non-empty list",
            message: "List was empty",
            value: json,
          })
        }
      | Result.Error(_) as err => err
      };
    };

  let codec: Decco.codec(_t) = (encoder, decoder);

  [@decco]
  type t = [@decco.codec codec] _t;
};

module NonEmptyListOfInt = {
  type _t = NonEmptyList.t(int);

  let encoder: Decco.encoder(_t) =
    lst => Decco.listToJson(Decco.intToJson, lst->NonEmptyList.toT);

  let decoder: Decco.decoder(_t) =
    json => {
      switch (Decco.listFromJson(Decco.intFromJson, json)) {
      | Result.Ok(v) =>
        switch (NonEmptyList.fromT(v)) {
        | Some(vv) => Ok(vv)
        | None =>
          Result.Error({
            path: "decoding non-empty list",
            message: "List was empty",
            value: json,
          })
        }
      | Result.Error(_) as err => err
      };
    };

  let codec: Decco.codec(_t) = (encoder, decoder);

  [@decco]
  type t = [@decco.codec codec] _t;
};

module IntFromString = {
  let encoder: Decco.encoder(int) = i => Decco.intToJson(i);

  let decoder: Decco.decoder(int) =
    json => {
      switch (Decco.stringFromJson(json)) {
      | Result.Ok(v) => int_of_string(v)->Ok
      | Result.Error(_) as err => err
      };
    };

  let codec: Decco.codec(int) = (encoder, decoder);

  [@decco]
  type t = [@decco.codec codec] int;
};

module Buffer = {
  let codec: Decco.codec(Node.Buffer.t) = Decco.Codecs.magic;

  [@decco]
  type t = [@decco.codec codec] Node.Buffer.t;
};

let asIs = json => {
  json->Result.Ok;
};

let ignore = _json => Result.Ok();

let decodeLegacy = unsafeLegacy;