// Update with your config settings.
module.exports = {
  development: {
    client: "pg",
    connection: "postgres://postgres:verysecret@localhost:5432/postgres",
    pool: {
      min: process.env.DATABASE_POOL_MIN || 2,
      max: process.env.DATABASE_POOL_MAX || 20,
    },
    migrations: {
      tableName: "knex_migrations",
    },
  },

  production: {
    client: "pg",
    connection: `postgres://postgres:${process.env.POSTGRES_PASSWORD}@postgres:5432/postgres`,
    pool: {
      min: 2,
      max: 10,
    },
    migrations: {
      tableName: "knex_migrations",
    },
    ssl: true,
  },
};
