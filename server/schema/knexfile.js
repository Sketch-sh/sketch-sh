// Update with your config settings.
module.exports = {
  development: {
    client: 'sqlite3',
    connection: {
      filename: "file:memDb1?mode=memory&cache=shared"
      flags: ['OPEN_URI', 'OPEN_SHAREDCACHE']
    }
  }
}
  