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

module Hooked_console = {
  // let make_hooked_console_method =
  //   (. method) => {
  //     (. _log: ConsoleFeed.raw_data) => {
  //       let arguments = [%bs.raw {|[].slice.call(arguments)|}];
  //       ConsoleFeed.parse(method, arguments)
  //       ->ConsoleFeed.encode
  //       ->Comm_send_log
  //       ->Frame_comm.to_host;
  //     };
  //   };
  // let console = {
  //   "log": make_hooked_console_method(. "log"),
  //   "debug": make_hooked_console_method(. "debug"),
  //   "info": make_hooked_console_method(. "info"),
  //   "warn": make_hooked_console_method(. "warn"),
  //   "error": make_hooked_console_method(. "error"),
  //   "table": make_hooked_console_method(. "table"),
  //   "clear": make_hooked_console_method(. "clear"),
  //   "time": make_hooked_console_method(. "time"),
  //   "timeEnd": make_hooked_console_method(. "timeEnd"),
  //   "count": make_hooked_console_method(. "count"),
  //   "assert": make_hooked_console_method(. "assert"),
  // };
};

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

let get_protocol_pathname: string => option((string, string)) =
  url => {
    url
    ->String.match([%re "/(^[a-zA-Z\\-]+\\:)(?:\\/.)(.+)/"])
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
         | Npm =>
           if (pathname |> Js.String.startsWith("bs-platform@latest/lib/js/")) {
             let new_filesystem =
               pathname
               |> Js.String.replace("bs-platform@latest/lib/js", "stdlib");
             handle_bs_stdlib(~url, ~meta, ~pathname=new_filesystem);
           } else {
             Container_fetcher_npm.handle_npm(~url, ~meta, ~pathname);
           }

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

let make_polestar = (~onError) =>
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
        onError(error);
      },
  });

let clean_dom: unit => unit = [%raw
  () => {|
  document.body.innerHTML = '<div id="root"></div>'
|}
];

let eval = (polestar, code) => {
  let dependencies = Container_require_collector.parse(code);
  clean_dom();

  let _ =
    polestar->Polestar.evaluate(dependencies, code)
    |> then_(_result => resolve());
  ();
};
