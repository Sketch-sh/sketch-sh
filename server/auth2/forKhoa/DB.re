open Pom;

exception DatabaseError(string);

module Bricks = SqlBricks;
module Abstract = DB_Abstract;

module Stats = {
  let statsPool = Pg.Pool.make({"connectionString": Config.statsUrl});

  let perform = statsPool->Abstract.perform;
  let performRaw = (~values=?, text): pom(Js.Json.t) => {
    Abstract.performRaw(statsPool, ~values?, text);
  };
};

let pool = Pg.Pool.make({"connectionString": Config.postgresUrl});

let perform = pool->Abstract.perform;
let performRaw = (~values=?, text): pom(Js.Json.t) => {
  Abstract.performRaw(pool, ~values?, text);
};

module Syncable =
  DB_Table.Table({
    let name = "syncable";
    let pool = pool;
  });

module Account =
  DB_Table.Table({
    let name = "account";
    let pool = pool;
  });

module Gift =
  DB_Table.Table({
    let name = "gift";
    let pool = pool;
  });

module Attachment =
  DB_Table.Table({
    let name = "attachment";
    let pool = pool;
  });

module Inmail =
  DB_Table.Table({
    let name = "inmail";
    let pool = pool;
  });

module StatsEvents =
  DB_Table.Table({
    let name = "event";
    let pool = Stats.statsPool;
  });

module OldIftttToken =
  DB_Table.Table({
    let name = "old_ifttt_token";
    let pool = pool;
  });

module Print =
  DB_Table.Table({
    let name = "print";
    let pool = pool;
  });