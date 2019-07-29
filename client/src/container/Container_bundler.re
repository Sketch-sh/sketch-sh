module Polestar = Container_polestar;
module B = Container_bindings;
module Url = Webapi.Url;

let url_join = (~base, ~path) => {
  let base = Url.make(base);
  let new_url = Url.makeWithBase(path, base->Url.href);

  new_url->Url.href;
};

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

  let handle_filesystem = (~url, ~meta: _, ~pathname as filename) => {
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
        Js.log(code);
        resolve(.
          Polestar.Fetcher.FetchResult.make(
            ~url,
            ~id=url,
            ~dependencies=Array(Container_require_collector.parse(code)),
            ~code,
            (),
          ),
        );
      }
    );
  };

  let handle_bs_stdlib = (~url, ~meta, ~pathname) => {
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

  let normalize_pathname = url => {
    Js.String.replaceByRe([%re "/^(\\/.)/g"], "", url);
  };
  // let parse
  let fetcher =
    (. url, meta: Polestar.Fetcher.meta) => {
      let parsed_url = Webapi.Url.make(url);
      let protocol = parsed_url->Webapi.Url.protocol->Protocol.of_string;
      let pathname = parsed_url->Webapi.Url.pathname->normalize_pathname;

      switch (protocol) {
      | Anonymous
      | Vfs =>
        if (pathname |> Js.String.startsWith("stdlib")) {
          handle_bs_stdlib(~url, ~meta, ~pathname);
        } else {
          handle_filesystem(~url, ~meta, ~pathname);
        }
      | _ =>
        Js.log(url);
        Js.Promise.make((~resolve, ~reject) => failwith("Unhandle case"));
      };
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

let eval = code => {
  open Js.Promise;
  let dependencies = Container_require_collector.parse(code);

  let _ =
    polestar->Polestar.evaluate(dependencies, code)
    |> then_(_result => resolve())
    |> catch(exn => {
         Js.log(exn);
         resolve();
       });
  ();
};
