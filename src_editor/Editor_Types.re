module Block = {
  type bcode = {
    bc_value: string,
    bc_firstLineNumber: int,
    bc_widgets: array(Editor_CodeBlockTypes.Widget.t),
  };

  type id = string;
  type blockType =
    | B_Code(bcode)
    | B_Text(string);

  type block = {
    b_id: id,
    b_data: blockType,
  };
};
