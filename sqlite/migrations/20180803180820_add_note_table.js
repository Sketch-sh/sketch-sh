exports.up = function (knex) {
  return Promise.all([
    /* Table: note */
    knex.schema.createTable("note", function (table) {
      table.string("id", 22).notNullable().primary();
      table.string("user_id", 22).notNullable().references("user.id");

      table.text("title");
      table.json("data");

      table.timestamp("created_at").notNullable().defaultTo(knex.fn.now());
      table.timestamp("updated_at").notNullable().defaultTo(knex.fn.now());
    }),
    /* Table: note_revision */
    knex.schema.createTable("note_revision", function (table) {
      table.string("note_id", 22).notNullable().references("note.id");
      table.timestamp("created_at").notNullable().defaultTo(knex.fn.now());

      table.primary(["note_id", "created_at"]);

      table.text("title");
      table.json("data");
    }),
  ]);
};

exports.down = function (knex) {
  return knex.raw(`
    DROP TABLE IF EXISTS "note" CASCADE;
    DROP TABLE IF EXISTS note_revision CASCADE;
  `);
};
