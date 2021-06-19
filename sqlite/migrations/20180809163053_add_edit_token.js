exports.up = function (knex) {
  return knex.schema.createTable("note_edit_token", function (table) {
    table.string("token", 22).notNullable();
    table.string("note_id", 22).notNullable().references("note.id");
    table.primary(["token", "note_id"]);
  });
};

exports.down = function (knex) {
  return knex.raw(`
    DROP TABLE IF EXISTS "note_edit_token" CASCADE;
  `);
};
