type options = {
  .
  // "dependencies": Js.Dict.t(string),
  "url": string,
};

[@bs.module "@frontarm/demoboard-worker/src/fetchDependency.ts"]
  external fetchDependency: (options) => Js.Promise.t(Sketch.Container_polestar.Fetcher.FetchResult.t) = "fetchDependency";

let handle_npm = (~url, ~meta as _, ~pathname as _) => {
  fetchDependency({
    "url": url
  })
}
