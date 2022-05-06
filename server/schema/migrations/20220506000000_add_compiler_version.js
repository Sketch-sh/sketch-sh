exports.up = function (knex, Promise) {
  return knex.raw(`
    ALTER TABLE note 
    ADD compiler_version VARCHAR(255) NOT NULL DEFAULT '4.06';
  `);
};

exports.down = function (knex, Promise) {
  return knex.raw(`
    ALTER TABLE note
    DROP COLUMN compiler_version;`);
};
