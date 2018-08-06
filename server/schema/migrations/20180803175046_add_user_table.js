exports.up = function(knex, Promise) {
  return Promise.all([
    /* Table: user */
    knex.schema.createTable("user", function(table) {
      table.string("id", 22).primary();
      table.text("username").notNullable();
      table.string("name");
      table.string("email");
      table.string("avatar");
      table
        .timestamp("created_at")
        .notNullable()
        .defaultTo(knex.fn.now());
      table
        .timestamp("updated_at")
        .notNullable()
        .defaultTo(knex.fn.now());
    }),
    knex.schema.createTable("user_identity_type", function(table) {
      table.string("user_identity_type").primary();
    }),
    knex.schema.createTable("user_identity", function(table) {
      table
        .string("user_id", 22)
        .notNullable()
        .references("user.id");
      table
        .string("identity_type")
        .notNullable()
        .references("user_identity_type.user_identity_type");

      table.primary(["user_id", "identity_type"]);
      table
        .string("identity_id")
        .notNullable()
        .comment("Id returns by identity providers");
      table.jsonb("data").notNullable();
    }),
  ]);
};

exports.down = function(knex, Promise) {
  return Promise.all([
    knex.schema.dropTableIfExists("user"),
    knex.schema.dropTableIfExists("user_identity"),
    knex.schema.dropTableIfExists("user_identity_type"),
  ]);
};
