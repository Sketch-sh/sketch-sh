module Polestar = Container_polestar;
module B = Container_bindings;

module Protocol = {
  type t =
    | Npm
    | Vfs
    | Annonymous
    | Unknown(string);

  let of_string =
    fun
    | "npm" => Npm
    | "vfs" => Vfs
    | "polestar-annonymous" => Annonymous
    | p => Unknown(p);
};

module Sketch_polestar = {
  let globals = {
    "process": {
      "env": {
        "NODE_ENV": "production",
      },
    },
  };

  // let parse
  let fetcher =
    (. url, meta) => {
      let parsed_url = Webapi.Url.make(url);
      let protocol = parsed_url->Webapi.Url.protocol->Protocol.of_string;

      Js.log(protocol);

      Js.Promise.make((~resolve, ~reject) => failwith("nothing"));
    };

  let polestar =
    Polestar.make({
      "globals": globals,
      "moduleThis": B.window,
      "fetcher": fetcher,
      "resolver": Polestar.defaultResolver,
      "onEntry":
        (.) => {
          Js.log("onEntry");
        },
      "onError":
        (. error) => {
          Js.log(error);
        },
    });
};

let polestar = Sketch_polestar.polestar;
