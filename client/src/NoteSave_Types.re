type id = string;
type noteKind =
  | New
  | Old(id);

type state = {kind: noteKind};

type noteSaveStatus =
  | NoteSave_Done
  | NoteSave_Loading
  | NoteSave_Error;
