const { join } = require("path");

const dbPath = join(__dirname, "../data/sketch_without_revision.db");
const db = require("better-sqlite3")(dbPath, { verbose: console.log });

db.exec("pragma foreign_keys = on;");

module.exports = db;
