exports.up = function(knex, Promise) {
  return knex.raw(`
    CREATE VIEW user_public AS
    SELECT "id", username, name, avatar, created_at, updated_at
    FROM "user";
    
    comment on view user_public is 'Publicly available user information';
  `);
};

exports.down = function(knex, Promise) {
  return knex.raw(`DROP VIEW IF EXISTS user_public`);
};
