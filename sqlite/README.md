# Covert data from PostgreSQL to Sqlite3

Just run these commands, replace `DATABASE_URL` with your PostgreSQL connection

```sh
export DATABASE_URL="postgres://USER:PASSWORD@HOST:PORT/DATABASE_NAME"
docker build . -t db-to-sqlite
docker run --rm \
           -v $(PWD)/data:/workdir \
           --network=host \
           db-to-sqlite \
           db-to-sqlite $DATABASE_URL sketch_without_revision.db --progress --all --skip=note_revision
```

Note: If you're macOS and Postgres database is running in host, use `host.docker.internal` for talking to host instead of localhost

Ideally, you can run the above command another time for `note_revision` table but it's quite large (747MB) and db-to-sqlite often hangs on that table so I prefer to export it to JSON format with `psql` 

```sh
psql

postgres=# \c sketch 
You are now connected to database "sketch" as user "postgres".
sketch=# COPY(SELECT row_to_json(t) FROM note_revision as t) to 'ABSOLUTE_PATH/note_revision.json';
sketch=# \q
```

