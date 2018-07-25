open Utils;

let component = ReasonReact.statelessComponent("Editor_Entry");

let code = [|Editor_Loremipsum.code1, Editor_Loremipsum.code2|];

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
         (Belt.Array.concat(acc, [|block|]), lines + firstLineNumber);
       },
     )
  |. (((acc, _firstLineNumber)) => acc);

let make = _children => {
  ...component,
  render: _self => <Editor_Page blocks />,
};

let default =
  ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
