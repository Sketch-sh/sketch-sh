open Pom;
open Handler;

let handler = _req => {
  resolved @@ OkString("ok");
};

module Endpoint =
  Handler.Make({
    let path = "/health";
    let verb = GET;

    let handler = handler;
  });
