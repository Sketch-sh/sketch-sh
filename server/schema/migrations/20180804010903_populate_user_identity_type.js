exports.up = function(knex, Promise) {
  return knex("user_identity_type").insert([
    { user_identity_type: "github" },
    { user_identity_type: "passwordless" },
  ]);
};

exports.down = function(knex, Promise) {
  return new Promise(resolve => resolve());
};
