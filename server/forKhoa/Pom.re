module Bluebird = {
  %bs.raw
  "var Promise = require('bluebird')";

  %bs.raw
  "Promise.config({ warnings: false })";

  type t('a);

  [@bs.module "bluebird"]
  external fromJs: Js.Promise.t('a) => t('a) = "resolve";

  let toJs: t('a) => Js.Promise.t('a) = Obj.magic;

  [@bs.module "bluebird"] external resolve: 'a => t('a) = "";
  [@bs.module "bluebird"] external reject: 'a => t('a) = "";
  [@bs.module "bluebird"] external all: array(t('a)) => t(array('a)) = "";

  [@bs.send] external flatMap: (t('a), 'a => t('b)) => t('b) = "then";
  [@bs.send] external tap: (t('a), 'a => unit) => t('a) = "";
  [@bs.send]
  external tapCatch: (t('a), Js.Promise.error => unit) => t('a) = "";
  [@bs.send]
  external catch: (t('a), Js.Promise.error => t('b)) => t('b) = "";
};

type pom('a) = Bluebird.t('a);
type pomWithError('a, 'err) = Bluebird.t(Result.t('a, 'err));

// Use this function to turn a callback into a promise.
// Call it to get back a tuple of (p, re).
// You can use the "p", which is a promise, as the return value of your function,
// and use the "re" function to resolve the function with some value inside of
// your callback.
let make = (): (pom('a), 'a => unit, exn => unit) => {
  let resolver = ref(ignore);
  let rejecter = ref(ignore);
  let p =
    Js.Promise.make((~resolve, ~reject) => {
      resolver := (a => resolve(. a));
      rejecter := (a => reject(. a));
    });

  (p->Bluebird.fromJs, resolver^, rejecter^);
};

let makeWithCallback = () => {
  let (p, resolve, reject) = make();
  let isFalsy: 'x => bool = [%bs.raw a => {| return a ? true : false|}];
  let callback = (err: 'e, result: 'a) =>
    /* The convention with Node or JS callbacks is to
       check an error like this:
       if (err) {
         ...
       } else {
         ...
       }

       Thus, some JS libraries may return false, null, 0, or something other than undefined
       when there was no error. We'll be loose with our test to account for this.
        */
    isFalsy(err) ? reject(err) : resolve(result);
  (p, callback);
};

type nodeCallback('data) = (exn, 'data) => unit;
let fromCallback = (fn: nodeCallback('data) => unit): pom('data) => {
  let (p, cb) = makeWithCallback();
  fn(cb);
  p;
};

let resolved: 'a => pom('a) = Bluebird.resolve;

let map = (p: pom('a), mapper: 'a => 'b): pom('b) =>
  p->Bluebird.flatMap(v => mapper(v)->Bluebird.resolve);

let tap = Bluebird.tap;
let tapCatch = Bluebird.tapCatch;

let flatMap = (p: pom('a), mapper: 'a => pom('b)): pom('b) =>
  p->Bluebird.flatMap(v => mapper(v));

exception PromiseRejected;
let recover = (p: pom('a), mapper: Js.Promise.error => 'a): pom('a) =>
  p->Bluebird.catch(e => mapper(e)->resolved);
let catch = (p: pom('a), mapper: Js.Promise.error => pom('a)): pom('a) =>
  p->Bluebird.catch(mapper);
let onError = (p: pom('a), effect: Js.Promise.error => unit): pom('a) =>
  p->catch(err => {
    effect(err);
    raise(PromiseRejected);
  });

// For using when you want to perform some effect on the result of a function, instead of
// just transforming the value. (For example, printing the result to the console).
let wait = (p: pom('a), waiter: 'a => unit): pom(unit) =>
  p->map(v => waiter(v));

let all: array(pom('a)) => pom(array('a)) = Bluebird.all;

module WithError = {
  let wrapOk = (a: 'a): pomWithError('a, 'err) =>
    resolved(Belt.Result.Ok(a));
  let wrapError = (err: 'err): pomWithError('a, 'err) =>
    resolved(Belt.Result.Error(err));

  let mapOk =
      (p: pomWithError('a, 'err), mapper: 'a => 'b): pomWithError('b, 'err) => {
    p->map(
      fun
      | Belt.Result.Ok(a) => Belt.Result.Ok(mapper(a))
      | Belt.Result.Error(_) as r => r,
    );
  };

  let flatMapOk =
      (p: pomWithError('a, 'err), mapper: 'a => pomWithError('b, 'err))
      : pomWithError('b, 'err) => {
    p->flatMap(
      fun
      | Belt.Result.Ok(a) => mapper(a)
      | Belt.Result.Error(_) as r => r->resolved,
    );
  };

  let mapError =
      (p: pomWithError('a, 'err), mapper: 'err => 'err2)
      : pomWithError('a, 'err2) => {
    p->map(
      fun
      | Belt.Result.Ok(_) as r => r
      | Belt.Result.Error(e) => Belt.Result.Error(mapper(e)),
    );
  };

  let flatMapError =
      (p: pomWithError('a, 'err), mapper: 'err => pomWithError('a, 'err2))
      : pomWithError('a, 'err2) => {
    p->flatMap(
      fun
      | Belt.Result.Ok(_) as r => r->resolved
      | Belt.Result.Error(e) => mapper(e),
    );
  };

  let recover =
      (p: pomWithError('a, 'err), recoverer: 'err => pomWithError('a, 'err))
      : pomWithError('b, 'err2) => {
    p->flatMap(
      fun
      | Belt.Result.Ok(_) as r => r->resolved
      | Belt.Result.Error(e) => recoverer(e),
    );
  };
};

let fromJsPromise = Bluebird.fromJs;
let toJsPromise = Bluebird.toJs;
let optionify = p => p->map(a => Some(a));

module Infix = {
  let (<$>) = map;
  let (>>=) = flatMap;
  let (<!>) = tapCatch;
};
include Infix;

let let_ = flatMap;
module Wrap = {
  let let_ = map;
};
module AsJS = {
  let let_ = (p, cb) => flatMap(p, cb)->toJsPromise;
};
module Effect = {
  let let_ = (p, cb) => flatMap(p, cb) |> ignore;
  module Wrap = {
    let let_ = (p, cb) => map(p, cb) |> ignore;
  };
};