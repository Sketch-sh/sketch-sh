open Jest;

open CodeMirror_Links;
let parse = Js.Json.parseExn;

describe("linkCollector", () => {
  open Expect;
  open! Expect.Operators;
  let data = [%bs.raw
    {|
     [{start: 0, end: 1, string: "t", type: null, state: null},
      {start: 1, end: 2, string: "h", type: null, state: null},
      {start: 2, end: 3, string: "i", type: null, state: null},
      {start: 3, end: 4, string: "s", type: null, state: null},
      {start: 4, end: 5, string: " ", type: null, state: null},
      {start: 5, end: 10, string: "https", type: "link", state: null},
      {start: 10, end: 11, string: ":", type: "link", state: null},
      {start: 11, end: 22, string: "//sketch.sh", type: "link", state: null},
      {start: 22, end: 23, string: " ", type: null, state: null},
      {start: 23, end: 24, string: "i", type: null, state: null},
      {start: 24, end: 25, string: "s", type: null, state: null},
      {start: 25, end: 26, string: " ", type: null, state: null},
      {start: 26, end: 27, string: "a", type: null, state: null},
      {start: 27, end: 28, string: " ", type: null, state: null},
      {start: 28, end: 29, string: "l", type: null, state: null},
      {start: 29, end: 30, string: "i", type: null, state: null},
      {start: 30, end: 31, string: "n", type: null, state: null},
      {start: 31, end: 32, string: "k", type: null, state: null}]
    |}
  ];
  test("1 link", () =>
    expect(linkCollector(data, ~line=1))
    == [
         {
           colStart: 5,
           colEnd: 22,
           content: "https://sketch.sh",
           line: 1,
           withTitle: false,
         },
       ]
  );

  let data = [%bs.raw
    {|
     [
       {"start":0,"end":1,"string":"t","type":null,"state":null},
       {"start":1,"end":2,"string":"h","type":null,"state":null},
       {"start":2,"end":3,"string":"i","type":null,"state":null},
       {"start":3,"end":4,"string":"s","type":null,"state":null},
       {"start":4,"end":5,"string":" ","type":null,"state":null},
       {"start":5,"end":10,"string":"https","type":"link","state":null},
       {"start":10,"end":11,"string":":","type":"link","state":null},
       {"start":11,"end":22,"string":"//sketch.sh","type":"link","state":null},
       {"start":22,"end":23,"string":" ","type":null,"state":null},
       {"start":23,"end":24,"string":"i","type":null,"state":null},
       {"start":24,"end":25,"string":"s","type":null,"state":null},
       {"start":25,"end":26,"string":" ","type":null,"state":null},
       {"start":26,"end":27,"string":"a","type":null,"state":null},
       {"start":27,"end":28,"string":" ","type":null,"state":null},
       {"start":28,"end":29,"string":"l","type":null,"state":null},
       {"start":29,"end":30,"string":"i","type":null,"state":null},
       {"start":30,"end":31,"string":"n","type":null,"state":null},
       {"start":31,"end":32,"string":"k","type":null,"state":null},
       {"start":32,"end":33,"string":" ","type":null,"state":null},
       {"start":33,"end":38,"string":"https","type":"link","state":null},
       {"start":38,"end":39,"string":":","type":"link","state":null},
       {"start":39,"end":54,"string":"//google.com.vn","type":"link","state":null}]
    |}
  ];
  test("with a link at the end", () =>
    expect(linkCollector(data, ~line=1))
    == [
         {
           colStart: 33,
           colEnd: 54,
           content: "https://google.com.vn",
           line: 1,
           withTitle: false,
         },
         {
           colStart: 5,
           colEnd: 22,
           content: "https://sketch.sh",
           line: 1,
           withTitle: false,
         },
       ]
  );

  let data = [%bs.raw
    {|
     [
      {start: 0, end: 1, string: "-", type: "variable-2", state: null},
      {start: 1, end: 2, string: " ", type: "variable-2", state: null},
      {start: 2, end: 3, string: "[", type: "link variable-2", state: null},
      {start: 3, end: 4, string: "t", type: "link variable-2", state: null},
      {start: 4, end: 5, string: "h", type: "link variable-2", state: null},
      {start: 5, end: 6, string: "a", type: "link variable-2", state: null},
      {start: 6, end: 7, string: "n", type: "link variable-2", state: null},
      {start: 7, end: 8, string: "g", type: "link variable-2", state: null},
      {start: 8, end: 9, string: "n", type: "link variable-2", state: null},
      {start: 9, end: 10, string: "g", type: "link variable-2", state: null},
      {start: 10, end: 11, string: "o", type: "link variable-2", state: null},
      {start: 11, end: 12, string: "c", type: "link variable-2", state: null},
      {start: 12, end: 13, string: "8", type: "link variable-2", state: null},
      {start: 13, end: 14, string: "9", type: "link variable-2", state: null},
      {start: 14, end: 15, string: "]", type: "link variable-2", state: null},
      {start: 15, end: 16, string: "(", type: "string url variable-2", state: null},
      {start: 16, end: 46, string: "https://github.com/thangngoc89", type: "string url variable-2", state: null},
      {start: 46, end: 47, string: ")", type: "string url variable-2", state: null}
     ]
    |}
  ];
  test("with multiple type in a token", () =>
    expect(linkCollector(data, ~line=1))
    == [
         {
           colStart: 2,
           colEnd: 47,
           content: "[thangngoc89](https://github.com/thangngoc89)",
           line: 1,
           withTitle: true,
         },
       ]
  );
});
