// Update with your config settings.

module.exports = {
  development: {
    client: "pg",
    connection: "postgres://postgres:mysecretpassword@localhost:5432/postgres",
    pool: {
      min: process.env.DATABASE_POOL_MIN || 2,
      max: process.env.DATABASE_POOL_MAX || 20,
    },
    migrations: {
      tableName: "knex_migrations",
    },
  },

  production: {
    client: "postgresql",
    connection: {
      database: "my_db",
      user: "username",
      password: "password",
    },
    pool: {
      min: 2,
      max: 10,
    },
    migrations: {
      tableName: "knex_migrations",
    },
  },
};
