exports.up = function(knex, Promise) {
  return knex("user").insert({
    id: "anonymous",
    username: "anonymous",
  });
};

exports.down = function(knex, Promise) {
  return new Promise(resolve => resolve());
};
