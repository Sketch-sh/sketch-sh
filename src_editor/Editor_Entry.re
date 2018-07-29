open Utils;

let component = ReasonReact.statelessComponent("Editor_Entry");

let code = [|
  Editor_Loremipsum.code1,
  /* Editor_Loremipsum.code2, */
  /* Editor_Loremipsum.code3, */
|];

open Editor_Types.Block;
let blocks =
  code
  |. Belt.Array.reduceU(
       ([||], 1),
       (. (acc, firstLineNumber), code) => {
         let lines = countLine(code);

         let block =
           B_Code({
             bc_value: code,
             bc_firstLineNumber: firstLineNumber,
             bc_widgets: [||],
           });
         (Belt.Array.concat(acc, [|block|]), lines + firstLineNumber + 1);
       },
     )
  |. Utils.pluckAcc;

let blocks =
  if (Utils.env == "production") {
    Belt.Array.concat([|B_Text(Editor_Loremipsum.text1)|], blocks);
  } else {
    blocks;
  };

let blocks =
  blocks
  |. Belt.Array.mapU((. b_data) => {b_id: Utils.generateId(), b_data});

let make = _children => {
  ...component,
  render: _self => <Editor_Page blocks />,
};

let default =
  ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
