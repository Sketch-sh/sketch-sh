let text1 = {|Hey. Welcome to [Sketch.sh](https://sketch.sh) - [Github](https://github.com/Sketch-sh/sketch-sh)
You are viewing an interactive ReasonML sketchbook.

Things you can do with Sketch.sh:

## Show runtime values

It can print runtime values without print function|};

let code2 = {|print_endline("Hello ReasonML folks");

let noPrintFuntionNeeded = [
  Random.int(10) > 4 ? Some("Hello") : None,
  None,
  Some("good bye")
];|};

let text3 = "## All code blocks are connected";

let code4 = "noPrintFuntionNeeded; /* This is from the first previous block */";

let text5 = {|## Inline errors and warnings|};

let code6 = {|type say = Hello | Goodbye;

fun
  | Hello => ()
  | Goodbye => ()
  | _ => ();

let a: string = 1;|};

let text7 = {|## Authors:

This project is brought to you by
- [thangngoc89](https://github.com/thangngoc89)

And contributors:

- [matthiaskern](https://github.com/matthiaskern)
- [no-stack-dub-sack](https://github.com/no-stack-dub-sack)
- [alireza-a](https://github.com/alireza-a)

Have funs!|};

let blocks = [|
  {"kind": "text", "value": text1},
  {"kind": "code", "value": code2},
  {"kind": "text", "value": text3},
  {"kind": "code", "value": code4},
  {"kind": "text", "value": text5},
  {"kind": "code", "value": code6},
  {"kind": "text", "value": text7},
|];

external toJson: 'a => Js.Json.t = "%identity";

let wrapInBlocks = blocks => {"blocks": blocks};
let blocks =
  blocks
  ->(Belt.Array.mapU((. data) => {"id": Utils.generateId(), "data": data}))
  ->wrapInBlocks
  ->toJson
  ->Editor_Json.V1.JsonDecode.decode;
