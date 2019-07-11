include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: "src/test/repl/__snapshots__",
      projectDir: "src"
    });
});
