exports.up = function(knex, Promise) {
  return knex.raw(`
    CREATE DOMAIN nanoid AS TEXT CHECK(VALUE ~ '^[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789]{22}$');
  `);
};

exports.down = function(knex, Promise) {
  return knex.raw(`
    DROP DOMAIN IF EXISTS nanoid;
  `);
};
