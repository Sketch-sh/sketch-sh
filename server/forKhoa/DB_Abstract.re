open Pom;
module Bricks = SqlBricks;

let _connect = (pool: Pg.pool): pom(Pg.Client.t) => {
  Pg.Client.connect(pool)->fromJsPromise;
};

let perform = (pool: Pg.pool, bricks: Bricks.builder): pom(Js.Json.t) =>
  pool->Pg.Pool.query(bricks->Bricks.toPayload)->map(a => a##rows);

let performRaw = (pool: Pg.pool, ~values=?, text): pom(Js.Json.t) => {
  let fragment =
    switch (values) {
    | Some(v) => Bricks.sqlWithValues(~text, ~values=v)
    | None => Bricks.sql(text)
    };
  pool
  ->Pg.Pool.query({"text": fragment##str, "values": fragment##vals})
  ->map(a => a##rows);
};