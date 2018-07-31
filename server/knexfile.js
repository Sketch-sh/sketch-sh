// Update with your config settings.

module.exports = {
  development: {
    client: "sqlite3",
    connection: {
      filename: "./data/dev.sqlite3",
    },
  },

  staging: {
    client: "sqlite3",
    connection: {
      filename: "./data/staging.sqlite3",
    },
  },

  production: {
    client: "sqlite3",
    connection: {
      filename: "./data/prod.sqlite3",
    },
  },
};
