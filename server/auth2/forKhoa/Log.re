module SerializeError = {
  [@bs.module] external serializeError: 'a => Js.Json.t = "serialize-error";
  let go = serializeError;
};

module Pino = {
  type instance;
  [@bs.module] external make: Js.t('a) => instance = "./chocolate-pino";
  [@bs.send] external trace: (instance, 'a) => unit = "trace";
  [@bs.send] external trace2: (instance, 'a, 'b) => unit = "trace";
  [@bs.send] external trace3: (instance, 'a, 'b, 'c) => unit = "trace";
  [@bs.send] external trace4: (instance, 'a, 'b, 'c, 'd) => unit = "trace";
  [@bs.send] external trace5: (instance, 'a, 'b, 'c, 'd, 'e) => unit = "trace";
  [@bs.send] external debug: (instance, 'a) => unit = "debug";
  [@bs.send] external debug2: (instance, 'a, 'b) => unit = "debug";
  [@bs.send] external debug3: (instance, 'a, 'b, 'c) => unit = "debug";
  [@bs.send] external debug4: (instance, 'a, 'b, 'c, 'd) => unit = "debug";
  [@bs.send] external debug5: (instance, 'a, 'b, 'c, 'd, 'e) => unit = "debug";
  [@bs.send] external info: (instance, 'a) => unit = "info";
  [@bs.send] external info2: (instance, 'a, 'b) => unit = "info";
  [@bs.send] external info3: (instance, 'a, 'b, 'c) => unit = "info";
  [@bs.send] external info4: (instance, 'a, 'b, 'c, 'd) => unit = "info";
  [@bs.send] external info5: (instance, 'a, 'b, 'c, 'd, 'e) => unit = "info";
  [@bs.send] external warn: (instance, 'a) => unit = "warn";
  [@bs.send] external warn2: (instance, 'a, 'b) => unit = "warn";
  [@bs.send] external warn3: (instance, 'a, 'b, 'c) => unit = "warn";
  [@bs.send] external warn4: (instance, 'a, 'b, 'c, 'd) => unit = "warn";
  [@bs.send] external warn5: (instance, 'a, 'b, 'c, 'd, 'e) => unit = "warn";
  [@bs.send] external error: (instance, 'a) => unit = "error";
  [@bs.send] external error2: (instance, 'a, 'b) => unit = "error";
  [@bs.send] external error3: (instance, 'a, 'b, 'c) => unit = "error";
  [@bs.send] external error4: (instance, 'a, 'b, 'c, 'd) => unit = "error";
  [@bs.send] external error5: (instance, 'a, 'b, 'c, 'd, 'e) => unit = "error";
};

let defaultOpts = {
  "redact": [|"req.headers.authorization"|],
  "useLevelLabels": true,
  "level": Config.isProduction ? "info" : "trace",
};

let instance = Pino.make(defaultOpts);

let trace = a => instance->Pino.trace(a->SerializeError.go);
let trace2 = (a, b) =>
  instance->Pino.trace2(a->SerializeError.go, b->SerializeError.go);
let trace3 = (a, b, c) =>
  instance->Pino.trace3(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
  );
let trace4 = (a, b, c, d) =>
  instance->Pino.trace4(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
  );
let trace5 = (a, b, c, d, e) =>
  instance->Pino.trace5(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
    e->SerializeError.go,
  );

let debug = a => instance->Pino.debug(a->SerializeError.go);
let debug2 = (a, b) =>
  instance->Pino.debug2(a->SerializeError.go, b->SerializeError.go);
let debug3 = (a, b, c) =>
  instance->Pino.debug3(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
  );
let debug4 = (a, b, c, d) =>
  instance->Pino.debug4(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
  );
let debug5 = (a, b, c, d, e) =>
  instance->Pino.debug5(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
    e->SerializeError.go,
  );

let info = a => instance->Pino.info(a->SerializeError.go);
let info2 = (a, b) =>
  instance->Pino.info2(a->SerializeError.go, b->SerializeError.go);
let info3 = (a, b, c) =>
  instance->Pino.info3(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
  );
let info4 = (a, b, c, d) =>
  instance->Pino.info4(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
  );
let info5 = (a, b, c, d, e) =>
  instance->Pino.info5(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
    e->SerializeError.go,
  );

let warn = a => instance->Pino.warn(a->SerializeError.go);
let warn2 = (a, b) =>
  instance->Pino.warn2(a->SerializeError.go, b->SerializeError.go);
let warn3 = (a, b, c) =>
  instance->Pino.warn3(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
  );
let warn4 = (a, b, c, d) =>
  instance->Pino.warn4(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
  );
let warn5 = (a, b, c, d, e) =>
  instance->Pino.warn5(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
    e->SerializeError.go,
  );

let error = a => instance->Pino.error(a->SerializeError.go);
let error2 = (a, b) =>
  instance->Pino.error2(a->SerializeError.go, b->SerializeError.go);
let error3 = (a, b, c) =>
  instance->Pino.error3(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
  );
let error4 = (a, b, c, d) =>
  instance->Pino.error4(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
  );
let error5 = (a, b, c, d, e) =>
  instance->Pino.error5(
    a->SerializeError.go,
    b->SerializeError.go,
    c->SerializeError.go,
    d->SerializeError.go,
    e->SerializeError.go,
  );