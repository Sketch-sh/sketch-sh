exports.up = function(knex, Promise) {
  return knex.raw(`
  CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
 `);
};

exports.down = function(knex, Promise) {
  return Promise.resolve(resolve => resolve());
};
