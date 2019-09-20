require("dotenv").config();
require("isomorphic-fetch");

const { app } = require("./src/Server.bs.js");
const logger = require("./src/bones/logger");
const { port } = require("./src/Config.bs");

const server = app.listen(port, function() {
  logger.info("Listening on port", port);
});

// quit on ctrl-c when running docker in terminal
process.on("SIGINT", function onSigint() {
  console.info(
    "Got SIGINT (aka ctrl-c in docker). Graceful shutdown ",
    new Date().toISOString()
  );
  shutdown();
});

// quit properly on docker stop
process.on("SIGTERM", function onSigterm() {
  console.info(
    "Got SIGTERM (docker container stop). Graceful shutdown ",
    new Date().toISOString()
  );
  shutdown();
});

// shut down server
function shutdown() {
  server.close(function onServerClosed(err) {
    if (err) {
      console.error(err);
      process.exitCode = 1;
    }
    process.exit();
  });
}
//
// need above in docker container to properly exit
//
