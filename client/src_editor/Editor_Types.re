type lang =
  | ML
  | RE;

let stringToLang =
  fun
  | "ML" => ML
  | "RE" => RE
  | _ => failwith("Unknown language");

let langToString =
  fun
  | ML => "ML"
  | RE => "RE";

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
  type bcode = {
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
    b_deleted: bool,
  };

  type deletedBlockMeta = {
    db_id: id,
    db_data: blockData,
    to_id: Js.Global.timeoutId,
  };

  type focusChangeType =
    | FcTyp_EditorFocus
    | FcTyp_BlockNew
    | FcTyp_BlockFocusUp
    | FcTyp_BlockFocusDown
    /* Shift+Enter shortcut */
    | FcTyp_BlockExecuteAndFocusNextBlock;
};
