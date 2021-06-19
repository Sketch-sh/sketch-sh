exports.up = function (knex) {
  return knex.schema.table("note", function (table) {
    table.string("fork_from", 22);
    table.bool("is_published").defaultTo(false).notNullable();
    table.string("slug");
  });
};

exports.down = function (knex) {
  return Promise.resolve((resolve) => resolve());
};
