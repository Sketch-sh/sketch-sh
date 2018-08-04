let blocks = [|
  {
    Editor_Types.Block.b_id: Utils.generateId(),
    b_data: Editor_Blocks_Utils.emptyCodeBlock(),
  },
|];

let component = ReasonReact.statelessComponent("Note_New");

let make = _children => {
  ...component,
  render: _self => <Editor_Note blocks />,
};
