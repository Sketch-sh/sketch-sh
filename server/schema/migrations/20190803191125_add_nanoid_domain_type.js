exports.up = function(knex, Promise) {
  const check = `~ '^[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789]{22}$'`;
  return knex.raw(`
    ALTER TABLE note ADD CONSTRAINT id_nanoid_check CHECK (id ${check});
  `);
};

exports.down = function(knex, Promise) {
  return knex.raw(`
    ALTER TABLE "note" DROP CONSTRAINT id_nanoid_check;
  `);
};
