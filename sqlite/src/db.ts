import { join } from "path";
import Database from "better-sqlite3";
import debug from "debug";

const logger = debug("sketch:db");

const dbPath = join(__dirname, "../data/dev.db");
const db = Database(dbPath, { verbose: logger });

db.exec("pragma foreign_keys = on;");

export { db };
