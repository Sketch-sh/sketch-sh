open Jest;

open Editor_Json.V1;
open Editor_Types;
open Block;

let parse = Js.Json.parseExn;
/*
 * NOTE: If you change JSON encode/decode
 * - NEVER change the json value of decode
 * - You can change the expected ReasonML code
 * this is to ensure that all saved data will be correctly
 * decoded in case of an API change
 * - Encode could be change as will
 */
describe("decode", () => {
  open Expect;
  open! Expect.Operators;
  test("with fallback lang to RE", () => {
    let (lang, links, blocks) =
      {|{
      "links": [],
      "blocks": [
        {
          "id": "1",
          "data" : {
            "kind": "code",
            "value": "let a: string = 1;"
          }
        },
        {
          "id": "2",
          "data" : {
            "kind": "text",
            "value": "awesome"
          }
        }
      ]
    }|}
      ->parse
      ->decode;

    expect((lang, links, blocks))
    == (
         RE,
         [||],
         [|
           {
             b_id: "1",
             b_data:
               B_Code({
                 bc_value: "let a: string = 1;",
                 bc_firstLineNumber: 1,
                 bc_widgets: [||],
               }),
             b_deleted: false,
           },
           {b_id: "2", b_data: B_Text("awesome"), b_deleted: false},
         |],
       );
  });

  test("decode lang correctly - ML", () => {
    let (lang, links, blocks) =
      {|{
      "lang": "ML",
      "links": [],
      "blocks": [
        {
          "id": "1",
          "data" : {
            "kind": "code",
            "value": "let a: string = 1;"
          }
        },
        {
          "id": "2",
          "data" : {
            "kind": "text",
            "value": "awesome"
          }
        }
      ]
    }|}
      ->parse
      ->decode;

    expect((lang, links, blocks))
    == (
         ML,
         [||],
         [|
           {
             b_id: "1",
             b_data:
               B_Code({
                 bc_value: "let a: string = 1;",
                 bc_firstLineNumber: 1,
                 bc_widgets: [||],
               }),
             b_deleted: false,
           },
           {b_id: "2", b_data: B_Text("awesome"), b_deleted: false},
         |],
       );
  });
  test("decode lang correctly - RE", () => {
    let (lang, links, blocks) =
      {|{
      "lang": "RE",
      "links": [],
      "blocks": [
        {
          "id": "1",
          "data" : {
            "kind": "code",
            "value": "let a: string = 1;"
          }
        },
        {
          "id": "2",
          "data" : {
            "kind": "text",
            "value": "awesome"
          }
        }
      ]
    }|}
      ->parse
      ->decode;

    expect((lang, links, blocks))
    == (
         RE,
         [||],
         [|
           {
             b_id: "1",
             b_data:
               B_Code({
                 bc_value: "let a: string = 1;",
                 bc_firstLineNumber: 1,
                 bc_widgets: [||],
               }),
             b_deleted: false,
           },
           {b_id: "2", b_data: B_Text("awesome"), b_deleted: false},
         |],
       );
  });
  test("decode deleted blocks", () => {
    let (lang, links, blocks) =
      {|{
      "lang": "RE",
      "links": [],
      "blocks": [
        {
          "id": "1",
          "data" : {
            "kind": "code",
            "value": "let a: string = 1;"
          },
          "deleted": true
        },
        {
          "id": "2",
          "data" : {
            "kind": "text",
            "value": "awesome"
          },
          "deleted": false
        }
      ]
    }|}
      ->parse
      ->decode;

    expect((lang, links, blocks))
    == (
         RE,
         [||],
         [|
           {
             b_id: "1",
             b_data:
               B_Code({
                 bc_value: "let a: string = 1;",
                 bc_firstLineNumber: 1,
                 bc_widgets: [||],
               }),
             b_deleted: true,
           },
           {b_id: "2", b_data: B_Text("awesome"), b_deleted: false},
         |],
       );
  });
});

describe("encode", () => {
  open Expect;
  open! Expect.Operators;
  test("encode with lang field - RE", () => {
    let json =
      {|{
      "lang": "RE",
      "links": [],
      "blocks": [
        {
          "id": "1",
          "data" : {
            "kind": "code",
            "value": "let a: string = 1;"
          },
          "deleted": false
        },
        {
          "id": "2",
          "data" : {
            "kind": "text",
            "value": "awesome"
          },
          "deleted": false
        }
      ]
    }|}
      ->parse;

    expect(
      encode(
        RE,
        [||],
        [|
          {
            b_id: "1",
            b_data:
              B_Code({
                bc_value: "let a: string = 1;",
                bc_firstLineNumber: 1,
                bc_widgets: [||],
              }),
            b_deleted: false,
          },
          {b_id: "2", b_data: B_Text("awesome"), b_deleted: false},
        |],
      ),
    )
    == json;
  });
  test("encode with lang field - ML", () => {
    let json =
      {|{
      "lang": "ML",
      "links": [],
      "blocks": [
        {
          "id": "1",
          "data" : {
            "kind": "code",
            "value": "let a: string = 1;"
          },
          "deleted": true
        },
        {
          "id": "2",
          "data" : {
            "kind": "text",
            "value": "awesome"
          },
          "deleted": false
        }
      ]
    }|}
      ->parse;

    expect(
      encode(
        ML,
        [||],
        [|
          {
            b_id: "1",
            b_data:
              B_Code({
                bc_value: "let a: string = 1;",
                bc_firstLineNumber: 1,
                bc_widgets: [||],
              }),
            b_deleted: true,
          },
          {b_id: "2", b_data: B_Text("awesome"), b_deleted: false},
        |],
      ),
    )
    == json;
  });
});
