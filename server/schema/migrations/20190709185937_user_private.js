exports.up = function(knex, Promise) {
  return knex.raw(`
    CREATE VIEW user_private AS
    SELECT "id", email
    FROM "user";
    
    comment on view user_private is 'Private information that belongs to user';
  `);
};

exports.down = function(knex, Promise) {
  return knex.raw(`DROP VIEW IF EXISTS user_private;`);
};
