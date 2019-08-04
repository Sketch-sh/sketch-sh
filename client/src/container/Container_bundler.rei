module Polestar = Container_polestar;

let make_polestar: (~onError: exn => unit) => Polestar.polestar;

let eval: (Polestar.polestar, string) => unit;
