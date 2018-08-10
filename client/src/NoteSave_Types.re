type id = string;
type noteKind =
  | New
  | Old(id);

type state = {kind: noteKind};

type noteSaveStatus =
  | NoteSave_Done
  | NoteSave_Loading
  | NoteSave_Error;

let noteSaveStatusToString =
  if (Utils.env == "production") {
    _noteSaveStatus => "";
  } else {
    noteSaveStatus =>
      switch (noteSaveStatus) {
      | NoteSave_Done => "NoteSave_Done"
      | NoteSave_Loading => "NoteSave_Loading"
      | NoteSave_Error => "NoteSave_Error"
      };
  };
