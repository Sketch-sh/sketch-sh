open SStdlib;

module Polestar = Container_polestar;
module B = Container_bindings;

module Config = {
  let bs_stdlib_url = "";
};

module Protocol = {
  type t =
    | Npm
    | Vfs
    | Anonymous
    | Unknown(string);

  let of_string =
    fun
    | "npm:" => Npm
    | "vfs:" => Vfs
    | "polestar-anonymous:" => Anonymous
    | p => Unknown(p);
};

module Sketch_polestar = {
  open Js.Promise;

  let globals = {
    "process": {
      "env": {
        "NODE_ENV": "production",
      },
    },
  };

  let handle_filesystem = (~url, ~meta as _, ~pathname as filename) => {
    let file_content = Container_fs.get(~filename);

    Js.Promise.make((~resolve, ~reject: _) =>
      switch (file_content) {
      | None =>
        reject(.
          {
            failwith("Unknown file " ++ filename);
          },
        )
      | Some(code) =>
        resolve(.
          Polestar.Fetcher.FetchResult.make(
            ~url,
            ~id=url,
            ~dependencies=Array(Container_require_collector.parse(code)),
            ~code,
            (),
          ),
        )
      }
    );
  };

  let handle_bs_stdlib = (~url, ~meta as _, ~pathname) => {
    Fetch.fetch(Config.bs_stdlib_url ++ "/" ++ pathname)
    |> then_(Fetch.Response.text)
    |> then_(code =>
         resolve(
           Polestar.Fetcher.FetchResult.make(
             ~url,
             ~id=url,
             ~dependencies=Array(Container_require_collector.parse(code)),
             ~code,
             (),
           ),
         )
       );
  };

  // npm://uuid@latest
  // => ("npm:", "uuid@latest")

  let get_protocol_pathname = url => {
    Js.String.match([%re "/(^[a-zA-Z]+\\:)(?:\\/.)(.+)/"], url)
    ->Belt.Option.flatMap(matches =>
        switch (matches->Belt.Array.length) {
        | 3 => Some((matches[1], matches[2]))
        | _ => None
        }
      );
  };
  // let parse
  let fetcher =
    (. url, meta: Polestar.Fetcher.meta) => {
      let parsed = get_protocol_pathname(url);

      Js.Promise.make((~resolve, ~reject) =>
        switch (parsed) {
        | None =>
          reject(. Promise.unsafe_reject("[Fetcher] Invalid url " ++ url))
        | Some(parsed) =>
          [%log.debug "parsed"; parsed];
          resolve(. parsed);
        }
      )
      |> Js.Promise.then_(((protocol, pathname)) =>
           switch (protocol |> Protocol.of_string) {
           | Anonymous
           | Vfs =>
             if (pathname |> Js.String.startsWith("stdlib")) {
               handle_bs_stdlib(~url, ~meta, ~pathname);
             } else {
               handle_filesystem(~url, ~meta, ~pathname);
             }
           | Npm => Container_fetcher_npm.handle_npm(~url, ~meta, ~pathname)
           | _ =>
             Js.Promise.make((~resolve as _, ~reject) =>
               reject(.
                 Promise.unsafe_reject(
                   "[Fetcher] Unhandle polestar url " ++ url,
                 ),
               )
             )
           }
         );
    };

  let polestar =
    Polestar.make({
      "globals": globals,
      "moduleThis": B.window,
      "fetcher": fetcher,
      "resolver": Polestar.defaultResolver,
      "onEntry":
        (.) => {
          (); // Should send back to host about execution done or something
            // Js.log("onEntry");
        },
      "onError":
        (. error) => {
          Js.log(error);
        },
    });
};

let polestar = Sketch_polestar.polestar;

let clean_dom: unit => unit = [%raw
  () => {|
  document.body.innerHTML = '<div id="root"></div>'
|}
];

let eval = code => {
  %log.debug
  "Eval";
  open Js.Promise;
  let dependencies = Container_require_collector.parse(code);

  clean_dom();

  let _ =
    polestar->Polestar.evaluate(dependencies, code)
    |> then_(_result => resolve());
  ();
};
