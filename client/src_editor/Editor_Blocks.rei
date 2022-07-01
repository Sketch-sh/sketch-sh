open Editor_Types;

type action =
  | Block_Add(id, Block.blockTyp)
  | Block_Execute(bool, Block.blockTyp)
  | Block_FocusNextBlockOrCreate(Block.blockTyp)
  | Block_QueueDelete(id)
  | Block_DeleteQueued(id)
  | Block_Restore(id)
  | Block_Focus(id, Block.blockTyp)
  | Block_Blur(id)
  | Block_UpdateValue(id, string, CodeMirror.EditorChange.t)
  | Block_AddWidgets(id, array(Widget.t))
  | Block_FocusUp(id)
  | Block_FocusDown(id)
  | Block_ChangeLanguage
  | Block_PrettyPrint
  | Block_CleanBlocksCopy
  | Block_MapRefmtToBlocks(list((id, string)));

module TimeoutMap = Belt.Map.String;

type state = {
  lang,
  compilerVersion: Sketch.CompilerVersion.t,
  blocks: array(Block.block),
  blocksCopy: option(array(Block.block)),
  deletedBlockMeta: ref(TimeoutMap.t(Js.Global.timeoutId)),
  stateUpdateReason: option(action),
  focusedBlock: option((id, Block.blockTyp, Block.focusChangeType)),
};

let make:
  (
    ~lang: lang=?,
    ~compilerVersion: Sketch.CompilerVersion.t,
    ~links: array(Link.link),
    ~blocks: array(Block.block),
    ~readOnly: bool=?,
    ~registerExecuteCallback: (unit => unit) => unit=?,
    ~registerShortcut: Shortcut.subscribeFun=?,
    ~onUpdate: array(Block.block) => unit,
    ~onExecute: bool => 'a
  ) =>
  React.element;
