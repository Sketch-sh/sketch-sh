type t;

[@bs.module] [@bs.new]
external make:
  {
    .
    "migrationDirectory": string,
    "connectionString": string,
    "driver": string,
    "schemaTable": string,
  } =>
  t =
  "postgrator";

type appliedMigration;
[@bs.send] external migrate: t => Js.Promise.t(array(appliedMigration)) = "";

let instance =
  make({
    "connectionString": Config.postgresUrl,
    "driver": "pg",
    "migrationDirectory": Filename.current_dir_name ++ "/migrations",
    "schemaTable": "private.migrations",
  });

%bs.raw
{|
instance
  .getMaxVersion()
  .then(version => console.log("Goal is to migrate to version", version))
  .catch(error => console.error("Failed getting max version from the FS", error));

// instance.on('validation-started', migration => console.log(migration))
// instance.on('validation-finished', migration => console.log(migration))
// instance.on('migration-started', migration => console.log(migration))
// instance.on('migration-finished', migration => console.log(migration))
|};

instance->migrate
|> Js.Promise.then_(appliedMigrations => {
     Log.info2("Migrations applied", appliedMigrations);
     Js.Promise.resolve();
   })
|> Js.Promise.catch(err => {
     Log.error2("Errors while applying migrations", err);
     [%bs.raw {|process.exit(1)|}] |> ignore;
     Js.Promise.resolve();
   });