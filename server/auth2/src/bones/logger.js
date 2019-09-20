const cls = require("cls-hooked");
const pino = require("pino");
const cuid = require("cuid");

const requestContext = cls.createNamespace("request");

let isProd = process.env.NODE_ENV == "production";

let defaultLogger = pino({
  prettyPrint: isProd
    ? false
    : {
        ignore: "pid,hostname",
        colorize: true,
        timestampKey: "time",
        levelFirst: true,
      },
  level: process.env.LOG_LEVEL || "info",
  redact: ["authorization", "token", "password", "req.headers.authorization"],
});

module.exports.middleware = getContext => (req, res, next) => {
  // GetContext should be a function that, given a request, returns
  // an object of information to add to any log statement after this.
  let context = getContext ? getContext(req) : {};

  // Automatically assign a request ID to the logger. If there isn't
  // one provided by the load balancer, make our own.
  let maybeReqId = req.get("X-Request-ID");
  let reqId = maybeReqId ? maybeReqId : cuid();
  context = Object.assign({ request_id: reqId }, context);

  const log = defaultLogger.child(context);
  req.log = log;

  requestContext.run(() => {
    // Set the logger on the async context, so that the global logging
    // function can use it when available.
    requestContext.set("logger", log);
    next();
  });
};

let getLogger = () => {
  const maybeLogger = requestContext.get("logger");
  return maybeLogger ? maybeLogger : defaultLogger;
};

module.exports.debug = (...args) => {
  getLogger().debug(...args);
};

module.exports.info = (...args) => {
  getLogger().info(...args);
};

module.exports.warn = (...args) => {
  getLogger().warn(...args);
};

module.exports.error = (...args) => {
  getLogger().error(...args);
};
