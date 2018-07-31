exports.up = async function(knex, Promise) {
  await Promise.all([
    knex.schema.createTable("user", function(table) {
      table.increments("id");
      table.string("username");
      table.timestamp("created_at").defaultTo(knex.fn.now());
      table.timestamp("updated_at").defaultTo(knex.fn.now());
    }),
    knex.schema.createTable("diare", function(table) {
      table.increments("id");
      table.string("public_id");
      table.string("title");
      table
        .integer("user_id")
        .unsigned()
        .references("user.id");
      table.timestamp("created_at").defaultTo(knex.fn.now());
      table.timestamp("updated_at").defaultTo(knex.fn.now());
    }),
    knex.schema.createTable("diare_revision", function(table) {
      table.integer("id").unsigned();
      table
        .integer("diare_id")
        .unsigned()
        .references("diare.id");
      table.primary(["id", "diare_id"]);
      table.string("file_path");
      table.timestamp("created_at").defaultTo(knex.fn.now());
    }),
  ]);
};

exports.down = async function(knex, Promise) {
  await Promise.all([
    knex.schema.dropTable("user"),
    knex.schema.dropTable("diare"),
    knex.schema.dropTable("diare_revision"),
  ]);
};
