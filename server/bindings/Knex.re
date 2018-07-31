type queryBuilder;

type t = (. string) => queryBuilder;

type querySql('options) = {
  .
  "method": string,
  "options": 'options,
  "bindings": array(string),
  "sql": string,
};

let fromTable = (string, knex: t) => knex(. string);

external toPromise: queryBuilder => Js.Promise.t(Js.Json.t) = "%identity";

[@bs.module] external make: KnexConfig.t => t = "knex";

[@bs.send.pipe: t] external raw: string => Js.Promise.t(Js.Json.t) = "";
[@bs.send.pipe: t]
external rawWithPositionalBindings:
  (string, array(Js.Json.t)) => Js.Promise.t(Js.Json.t) =
  "raw";
[@bs.send.pipe: t]
external rawWithBindingsObject: (string, Js.t('a)) => Js.Promise.t(Js.Json.t) =
  "raw";
[@bs.send.pipe: t] external destroy: unit = "";

[@bs.send.pipe: queryBuilder] external toSQL: unit => querySql('options) = "";

/* QueryBuilder Interface */
[@bs.send.pipe: queryBuilder] external select: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external as_: 'a => queryBuilder = "as";

[@bs.send.pipe: queryBuilder] external columns: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external column: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external from: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external into: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external table: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external distinct: 'a => queryBuilder = "";

/* Joins */
[@bs.send.pipe: queryBuilder]
external join: (string, Js.t('a)) => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external joinRaw: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external innerJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external innerJoinOp: (string, string, string, string) => queryBuilder =
  "innerJoin";

[@bs.send.pipe: queryBuilder]
external innerJoinFn: (string, queryBuilder => queryBuilder) => queryBuilder =
  "innerJoin";

[@bs.send.pipe: queryBuilder]
external leftJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external leftJoinOp: (string, string, string, string) => queryBuilder =
  "leftJoin";

[@bs.send.pipe: queryBuilder]
external leftJoinFn: (string, queryBuilder => queryBuilder) => queryBuilder =
  "leftJoin";

[@bs.send.pipe: queryBuilder]
external leftOuterJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external leftOuterJoinOp: (string, string, string, string) => queryBuilder =
  "leftOuterJoin";

[@bs.send.pipe: queryBuilder]
external leftOuterJoinFn:
  (string, queryBuilder => queryBuilder) => queryBuilder =
  "leftOuterJoin";

[@bs.send.pipe: queryBuilder]
external rightJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external rightJoinOp: (string, string, string, string) => queryBuilder =
  "rightJoin";

[@bs.send.pipe: queryBuilder]
external rightJoinFn: (string, queryBuilder => queryBuilder) => queryBuilder =
  "rightJoin";

[@bs.send.pipe: queryBuilder]
external rightOuterJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external rightOuterJoinOp: (string, string, string, string) => queryBuilder =
  "rightOuterJoin";

[@bs.send.pipe: queryBuilder]
external rightOuterJoinFn:
  (string, queryBuilder => queryBuilder) => queryBuilder =
  "rightOuterJoin";

[@bs.send.pipe: queryBuilder]
external outerJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external outerJoinOp: (string, string, string, string) => queryBuilder =
  "outerJoin";

[@bs.send.pipe: queryBuilder]
external outerJoinFn: (string, queryBuilder => queryBuilder) => queryBuilder =
  "outerJoin";

[@bs.send.pipe: queryBuilder]
external fullOuterJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external fullOuterJoinOp: (string, string, string, string) => queryBuilder =
  "fullOuterJoin";

[@bs.send.pipe: queryBuilder]
external fullOuterJoinFn:
  (string, queryBuilder => queryBuilder) => queryBuilder =
  "fullOuterJoin";

[@bs.send.pipe: queryBuilder]
external crossJoin: (string, string, string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external crossJoinOp: (string, string, string, string) => queryBuilder =
  "crossJoin";

[@bs.send.pipe: queryBuilder]
external crossJoinFn: (string, queryBuilder => queryBuilder) => queryBuilder =
  "crossJoin";

/* Withs */
[@bs.send.pipe: queryBuilder] external with_: 'a => queryBuilder = "with";

[@bs.send.pipe: queryBuilder] external withRaw: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external withSchema: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external withWrapped: 'a => queryBuilder = "";

/* Wheres */
[@bs.send.pipe: queryBuilder] external where: Js.t('a) => queryBuilder = "";
[@bs.send.pipe: queryBuilder]
external whereOperator: (string, string, 'a) => queryBuilder = "where";

[@bs.send.pipe: queryBuilder] external andWhere: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external andWhereOperator: (string, string, 'a) => queryBuilder = "andWhere";

[@bs.send.pipe: queryBuilder] external orWhere: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereNot: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external andWhereNot: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereNot: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external whereRaw: (string, array(Js.Json.t)) => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereRaw: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external andWhereRaw: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereWrapped: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external havingWrapped: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereExists: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereExists: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereNotExists: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external orWhereNotExists: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereIn: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereIn: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereNotIn: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereNotIn: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereNull: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereNull: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereNotNull: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereNotNull: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external whereBetween: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orWhereBetween: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external andWhereBetween: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external whereNotBetween: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external orWhereNotBetween: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external andWhereNotBetween: 'a => queryBuilder = "";

/* Group By */
[@bs.send.pipe: queryBuilder] external groupBy: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external groupByRaw: 'a => queryBuilder = "";

/* Order By */
[@bs.send.pipe: queryBuilder]
external orderBy: (string, [@bs.string] [ | `asc | `desc]) => queryBuilder =
  "";

[@bs.send.pipe: queryBuilder] external orderByRaw: 'a => queryBuilder = "";

/* Union */
[@bs.send.pipe: queryBuilder] external union: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external unionAll: 'a => queryBuilder = "";

/* Having */
[@bs.send.pipe: queryBuilder] external having: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external andHaving: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external havingRaw: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orHaving: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external orHavingRaw: 'a => queryBuilder = "";

/* Clear */
[@bs.send.pipe: queryBuilder] external clearSelect: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external clearWhere: 'a => queryBuilder = "";

/* Paging */
[@bs.send.pipe: queryBuilder] external offset: int => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external limit: int => queryBuilder = "";

/* Aggregation */
[@bs.send.pipe: queryBuilder]
external count: Js.Nullable.t(string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external countDistinct: Js.Nullable.t(string) => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external min: string => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external max: string => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external sum: string => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external sumDistinct: string => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external avg: string => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external avgDistinct: string => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external increment: (string, Js.Nullable.t(int)) => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external decrement: (string, Js.Nullable.t(int)) => queryBuilder = "";

/* Others */
[@bs.send.pipe: queryBuilder] external first: queryBuilder = "";

[@bs.send.pipe: queryBuilder] external debug: bool => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external pluck: string => queryBuilder = "";

[@bs.send.pipe: queryBuilder]
external insert_: ('a, Js.Nullable.t(string)) => queryBuilder = "insert";

[@bs.send.pipe: queryBuilder]
external update_: ('a, Js.Nullable.t(string)) => queryBuilder = "update";

[@bs.send.pipe: queryBuilder]
external updateSet: (string, 'a, Js.Nullable.t(string)) => queryBuilder =
  "update";

[@bs.send.pipe: queryBuilder] external returning: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external del: queryBuilder = "";

[@bs.send.pipe: queryBuilder] external truncate: unit => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external transacting: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external connection: 'a => queryBuilder = "";

[@bs.send.pipe: queryBuilder] external clone: unit => queryBuilder = "";

/* Transactions */
[@bs.send.pipe: t]
external transaction: (queryBuilder => Js.Promise.t('a)) => 'b = "";

/* Convenience Functions */
let insert = (~returning=Some("*"), data) =>
  insert_(data, Js.Nullable.fromOption(returning));

let update = (~returning=Some("*"), data) =>
  update_(data, Js.Nullable.fromOption(returning));

let delete = del;
