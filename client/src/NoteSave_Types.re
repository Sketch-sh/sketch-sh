type id = string;
type noteKind =
  | New
  | Old(id);

type state = {kind: noteKind};
