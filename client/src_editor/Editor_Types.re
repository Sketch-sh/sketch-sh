module Block = {
  type bcode = {
    bc_value: string,
    bc_firstLineNumber: int,
    bc_widgets: array(Editor_CodeBlockTypes.Widget.t),
  };

  type id = string;

  type blockData =
    | B_Code(bcode)
    | B_Text(string);

  type blockTyp =
    | BTyp_Code
    | BTyp_Text;

  type block = {
    b_id: id,
    b_data: blockData,
  };

  type focusChangeType =
    | FcTyp_EditorFocus
    | FcTyp_BlockFocusUp
    | FcTyp_BlockFocusDown;
};
