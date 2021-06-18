exports.up = function(knex, Promise) {
  return knex.raw(`
  create trigger trigger_note_revision
    before update
    on note
    for each row
  execute procedure trigger_on_note_revision();
  `);
};

exports.down = function(knex, Promise) {
  return knex.raw(`
    DROP TRIGGER IF EXISTS trigger_note_revision ON note;
  `);
};
