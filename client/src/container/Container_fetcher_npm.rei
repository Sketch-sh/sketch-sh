let handle_npm:
  (~url: string, ~meta: 'a, ~pathname: Js.String.t) =>
  Js.Promise.t(Sketch.Container_polestar.Fetcher.FetchResult.t);
