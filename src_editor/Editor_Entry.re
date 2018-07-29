open Utils;

let component = ReasonReact.statelessComponent("Editor_Entry");

let code = [|
  Editor_Loremipsum.code1,
  /* Editor_Loremipsum.code2, */
  Editor_Loremipsum.code3,
|];

let blocks =
  code
  |. Belt.Array.reduceU(
       ([||], 1),
       (. (acc, firstLineNumber), code) => {
         open Editor_Page;
         let lines = countLine(code);

         let block =
           B_Code({
             bc_value: code,
             bc_firstLineNumber: firstLineNumber,
             bc_lines: lines,
             bc_widgets: [||],
           });
         (Belt.Array.concat(acc, [|block|]), lines + firstLineNumber + 1);
       },
     )
  |. (((acc, _firstLineNumber)) => acc);

let blocks =
  if (Utils.env == "production") {
    Belt.Array.concat(
      [|Editor_Page.B_Text(Editor_Loremipsum.text1)|],
      blocks,
    );
  } else {
    blocks;
  };

let blocks =
  blocks
  |. Belt.Array.mapU((. b_data) =>
       {Editor_Page.b_id: Utils.generateId(), b_data}
     );
let make = _children => {
  ...component,
  render: _self => <Editor_Page blocks />,
};

let default =
  ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
