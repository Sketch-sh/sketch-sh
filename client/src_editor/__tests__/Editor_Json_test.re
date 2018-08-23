open Jest;

open Editor_Json.V1;
open Editor_Types;
open Block;

let parse = Js.Json.parseExn;

describe("decode", () => {
  open Expect;
  open! Expect.Operators;
  test("with fallback lang to RE", () => {
    let (lang, blocks) =
      {|{
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
      ->parse
      ->decode;

    expect((lang, blocks))
    == (
         RE,
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
    let (lang, blocks) =
      {|{
      "lang": "ML",
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
      ->parse
      ->decode;

    expect((lang, blocks))
    == (
         ML,
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
    let (lang, blocks) =
      {|{
      "lang": "RE",
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
      ->parse
      ->decode;

    expect((lang, blocks))
    == (
         RE,
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
});

describe("encode", () => {
  open Expect;
  open! Expect.Operators;
  test("encode with lang field - RE", () => {
    let json =
      {|{
      "lang": "RE",
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
        ML,
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
});
