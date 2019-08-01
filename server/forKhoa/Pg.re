// Hint, fold the code for readability, yeah?
type pool = {
  .
  "totalCount": int,
  "idleCount": int,
  "waitingCount": int,
};
type dbResult = {. "rows": Js.Json.t};
type values;
type queryPayload = {
  .
  "text": string,
  "values": values,
};

module Pool = {
  [@bs.module "pg"] [@bs.new]
  external make: {. "connectionString": string} => pool = "Pool";

  [@bs.send]
  external _query: (pool, queryPayload) => Js.Promise.t(dbResult) = "query";

  let query = (pool, payload) => _query(pool, payload)->Pom.fromJsPromise;
};

module Client = {
  type t;

  [@bs.send] external connect: pool => Js.Promise.t(t) = "";

  [@bs.send]
  external _query: (t, queryPayload) => Js.Promise.t(dbResult) = "query";
  let query = (client, payload) =>
    _query(client, payload)->Pom.fromJsPromise;

  [@bs.send] external release: t => unit = "";
};

module Events = {
  [@bs.send]
  external onConnect: (pool, [@bs.as "connect"] _, Client.t => unit) => unit =
    "on";

  [@bs.send]
  external onAcquire: (pool, [@bs.as "acquire"] _, Client.t => unit) => unit =
    "on";

  [@bs.send]
  external onError:
    (pool, [@bs.as "error"] _, (Js.Exn.t, Client.t) => unit) => unit =
    "on";

  [@bs.send]
  external onRemove: (pool, [@bs.as "remove"] _, Client.t => unit) => unit =
    "on";

  type poolStats = {
    totalCount: int,
    idleCount: int,
    waitingCount: int,
  };

  let poolStats = (pool: pool) =>
    "totalCount="
    ++ string_of_int(pool##totalCount)
    ++ ",idleCount="
    ++ string_of_int(pool##idleCount)
    ++ ",waitingCount="
    ++ string_of_int(pool##waitingCount)
    ++ ",";

  let poolStatsRec = (pool: pool) => {
    totalCount: pool##totalCount,
    idleCount: pool##idleCount,
    waitingCount: pool##waitingCount,
  };
};

module DbTypes = {
  type t;

  [@bs.val] external parseInt: string => int = "";
  [@bs.module "pg"] external types: t = "";
  [@bs.send] external setTypeParser: (t, int, string => 'a) => unit = "";
  [@bs.send]
  external setTypeParserD: (t, int, Js.Date.t => 'a) => unit = "setTypeParser";

  let parseDate = (a: Js.Date.t) => {
    Js.Date.toString(a);
  };

  types->setTypeParser(20, parseInt);
  types->setTypeParserD(1184, parseDate);
};