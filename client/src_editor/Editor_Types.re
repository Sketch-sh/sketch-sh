module Widget = {
  type lineWidgetData =
    | Lw_Error(string)
    | Lw_Warning(string)
    | Lw_Value(string)
    | Lw_Stdout(string);
  type t = {
    lw_line: int,
    lw_data: lineWidgetData,
  };
};

module Block = {
  type lang =
    | ML
    | RE;

  type bcode = {
    bc_lang: lang,
    bc_value: string,
    bc_firstLineNumber: int,
    bc_widgets: array(Widget.t),
  };

  type id = string;

  type blockData =
    | B_Code(bcode)
    | B_Text(string);

  type blockTyp =
    | BTyp_Code
    | BTyp_Text;

  let blockDataToBlockTyp =
    fun
    | B_Code(_) => BTyp_Code
    | B_Text(_) => BTyp_Text;

  type block = {
    b_id: id,
    b_data: blockData,
  };

  type focusChangeType =
    | FcTyp_EditorFocus
    | FcTyp_BlockNew
    | FcTyp_BlockFocusUp
    | FcTyp_BlockFocusDown
    /* Shift+Enter shortcut */
    | FcTyp_BlockExecuteAndFocusNextBlock;
};
