exports.up = function (knex) {
  return knex("user_identity_type").insert([{ user_identity_type: "github" }, { user_identity_type: "passwordless" }]);
};

exports.down = function (knex) {
  return new Promise((resolve) => resolve());
};
