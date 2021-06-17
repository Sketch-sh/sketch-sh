import { join } from "path"
import Database from "better-sqlite3";

const dbPath = join(__dirname, "../data/sketch_without_revision.db");
const db = Database(dbPath, { verbose: console.log });

db.exec("pragma foreign_keys = on;");

export { db }
