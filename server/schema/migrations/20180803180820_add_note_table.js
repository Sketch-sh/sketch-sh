exports.up = function(knex, Promise) {
  return Promise.all([
    /* Table: note */
    knex.schema.createTable("note", function(table) {
      table.string("id", 21).primary();
      table
        .string("user_id", 21)
        .notNullable()
        .references("user.id");

      table.text("title");
      table.jsonb("data");

      table
        .timestamp("created_at")
        .notNullable()
        .defaultTo(knex.fn.now());
      table
        .timestamp("updated_at")
        .notNullable()
        .defaultTo(knex.fn.now());
    }),
    /* Table: note_revision */
    knex.schema.createTable("note_revision", function(table) {
      table
        .string("note_id", 21)
        .notNullable()
        .references("note.id");
      table
        .timestamp("created_at")
        .notNullable()
        .defaultTo(knex.fn.now());

      table.primary(["note_id", "created_at"]);

      table.text("title");
      table.jsonb("data");
    }),
    knex.raw(`
    create or replace function trigger_on_note_revision()
    returns trigger
    language plpgsql as $body$
    begin

        -- Create revision only if node's subject or body columns have changed
        if old.title <> new.title or old."data" <> new."data" then
            insert into note_revision (note_id, created_at, title, "data")
            values (old.id, old.updated_at, old.title, old."data");
        end if;
        -- Return the NEW record so that update can carry on as usual
        return new;
    end; $body$;
    `),
  ]);
};

exports.down = function(knex, Promise) {
  return Promise.all([
    knex.raw("DROP TABLE IF EXISTS note CASCADE"),
    knex.raw("DROP TABLE IF EXISTS note_revision CASCADE"),
    knex.raw("DROP FUNCTION IF EXISTS trigger_on_note_revision CASCADE"),
  ]);
};
