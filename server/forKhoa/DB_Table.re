open Pom;

module type TableConfig = {
  let name: string;
  let pool: Pg.pool;
};

module Utils = {
  let getPgPath = (path: list(string)) => {
    "{" ++ (path |> List.toArray |> Js.Array.joinWith(",")) ++ "}";
  };

  let getStack = () => {
    let error: Js.Exn.t = [%bs.raw "new Error()"];
    Js.Exn.stack(error)->Option.getWithDefault("No trace could be provided");
  };
};

module Table = (CFG: TableConfig) => {
  module Bricks = SqlBricks;
  include Bricks.Raw;
  include Bricks.Types;
  include Bricks.Where;
  include Bricks.Filters;
  include Bricks.Transformers;
  include Bricks.OnConflict;
  include Bricks.To;

  let select = (columns: string): Bricks.builder => {
    Bricks.(select(columns)->from(CFG.name));
  };

  let selectIn = (~column: string, ~path: list(string)): Bricks.builder => {
    Bricks.select(column ++ "#>" ++ Utils.getPgPath(path))
    ->Bricks.from(CFG.name);
  };

  let update = (vs: Js.t('a)): Bricks.builder => {
    Bricks.(update(~table=CFG.name, vs));
  };
  let updateJson = (vs: Js.Json.t): Bricks.builder => {
    Bricks.(updateJson(~table=CFG.name, vs));
  };

  let setIn =
      (~column: string, ~path: list(string), value: 'a): Bricks.builder => {
    Bricks.updateTableOnly(CFG.name)
    ->Bricks.set(
        ~column,
        Bricks.sqlWithValues(
          ~text="jsonb_set($1, $2, $3)",
          ~values=`Three((column, Utils.getPgPath(path), value)),
        ),
      );
  };

  let deleteIn = (~column: string, ~path: list(string)): Bricks.builder => {
    Bricks.updateTableOnly(CFG.name)
    ->Bricks.set(
        ~column,
        Bricks.sqlWithValues(
          ~text="$1 #- $2)",
          ~values=`Two((column, Utils.getPgPath(path))),
        ),
      );
  };

  let _insert = (vs: 'a): Bricks.builder => {
    Bricks.(insertInto(CFG.name)->anyValue(vs));
  };
  let insert: Js.t('a) => Bricks.builder = _insert;
  let insertJson: Js.Json.t => Bricks.builder = _insert;

  let insertMany: array(Js.t('a)) => Bricks.builder = _insert;
  let insertManyJson: array(Js.Json.t) => Bricks.builder = _insert;

  let _upsert = (vs: 'a): Bricks.builder => {
    Bricks.(insertInto(CFG.name)->anyValue(vs))->onConflict("id")->doUpdate;
  };
  let upsert: Js.t('a) => Bricks.builder = _upsert;
  let upsertJson: Js.Json.t => Bricks.builder = _upsert;

  let delete = (): Bricks.builder => {
    Bricks.(delete()->from(CFG.name));
  };

  let perform = (builder, decoder: Decco.decoder('a)): pom('a) => {
    DB_Abstract.perform(CFG.pool, builder)->map(Codec.unsafe(decoder));
  };

  let maybeOne = (builder, decoder) => {
    let newDecoder = json => {
      Decco.arrayFromJson(decoder, json)
      ->Belt.Result.map(things => things->Belt.Array.get(0));
    };

    perform(builder, newDecoder);
  };

  let performExpectingNoResult = builder => {
    perform(builder, Codec.ignore);
  };

  open Belt.Result;
  let one = (builder, decoder) => {
    let newDecoder = json => {
      Decco.arrayFromJson(decoder, json)
      ->Belt.Result.flatMap(things =>
          switch (things->Belt.Array.get(0)) {
          | Some(v) => v->Ok
          | None =>
            {
              Decco.path: "[0]",
              message: "Expected an item to be in the head of the array, but found none",
              value: json,
            }
            ->Error
          }
        );
    };

    perform(builder, newDecoder);
  };
};