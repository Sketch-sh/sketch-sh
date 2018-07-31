importScripts("/berror.js");
importScripts("/reason.js");

const Comlink = require("comlink");

const obj = {
  execute: evaluator.execute,
  reset: evaluator.reset,
  reasonSyntax: evaluator.reasonSyntax,
  mlSyntax: evaluator.mlSyntax,
  parseError: berror.parse,
};

Comlink.expose(obj, self);
