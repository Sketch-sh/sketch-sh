exception UnknownLanguage;

type id = string;

type lang =
  | ML
  | RE;

let stringToLang =
  fun
  | "ML" => ML
  | "RE" => RE
  | _ => raise(UnknownLanguage);

let langToString =
  fun
  | ML => "ML"
  | RE => "RE";

let langToExtension =
  fun
  | ML => ".ml"
  | RE => ".re";

type noteState =
  | NoteState_New
  | NoteState_Old;

type editorContentStatus =
  | Ec_Pristine
  | Ec_Dirty
  | Ec_Saving
  | Ec_Saved;

type saveData = {lastEdited: Js.Json.t};
type saveStatus =
  | SaveStatus_Initial
  | SaveStatus_Loading
  | SaveStatus_Error
  | SaveStatus_Done(saveData);

type forkData = {
  newId: id,
  forkFrom: id,
  lastEdited: Js.Json.t,
  owner: id,
};
type forkStatus =
  | ForkStatus_Initial
  | ForkStatus_Loading
  | ForkStatus_Error
  | ForkStatus_Done(forkData);

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

  type focusChangeType =
    | FcTyp_EditorFocus
    | FcTyp_BlockNew
    | FcTyp_BlockFocusUp
    | FcTyp_BlockFocusDown
    /* Shift+Enter shortcut */
    | FcTyp_BlockExecuteAndFocusNextBlock;
};

module Link = {
  type internalLink = {
    revision_at: Js.Json.t,
    sketch_id: string,
    name: string,
    lang,
    code: string,
  };

  type externalLink;

  type link =
    | Internal(internalLink)
    | External(externalLink);
};
