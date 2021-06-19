exports.up = function (knex) {
  return knex("user").insert({
    id: "anonymous",
    username: "anonymous",
  });
};

exports.down = function (knex) {
  return new Promise((resolve) => resolve());
};
