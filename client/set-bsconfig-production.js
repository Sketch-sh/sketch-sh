#!/usr/bin/env node

const bsconfigPath = require.resolve("./bsconfig.json");
const originalConfig = require(bsconfigPath);

let modifiedConfig = Object.assign({}, originalConfig, {
  "bsc-flags": originalConfig["bsc-flags"].filter(a => a !== "-bs-g"),
});

const { writeFileSync } = require("fs");

writeFileSync(bsconfigPath, JSON.stringify(modifiedConfig, null, 2));
