open Worker_Types;

let compilerLocToCodeEditorLoc: CompilerErrorMessage.loc => loc =
  loc => {line: loc.o_line - 1, col: loc.o_col};

let compilerPosToPos:
  ((CompilerErrorMessage.loc, CompilerErrorMessage.loc)) => (loc, loc) =
  compilerPos => {
    let (from, to_) = compilerPos;
    (compilerLocToCodeEditorLoc(from), compilerLocToCodeEditorLoc(to_));
  };

let compilerErrorMessageToAbsolutePos =
    (content: CompilerErrorMessage.content, blockPos: (loc, loc)) => {
  let (blockFrom, _blockTo) = blockPos;
  let {line: blockFromLine} = blockFrom;
  let (from, to_) = content.o_pos;
  {
    ...content,
    o_pos: (
      {o_line: from.o_line + blockFromLine, o_col: from.o_col},
      {o_line: to_.o_line + blockFromLine, o_col: to_.o_col},
    ),
  };
};

let compilerErrorMessageToErrorMessage =
    (content: CompilerErrorMessage.content)
    : ErrorMessage.content => {
  errMsg_content: content.o_content,
  errMsg_pos: content.o_pos |> compilerPosToPos,
};
/**
 * Find a lower-bound index of a value in a sorted array of ranges.
 *
 * Assume `arr = [0, 5, 10, 15, 20]` and
 * this returns `1` for `value = 7` (5 <= value < 10),
 * and returns `3` for `value = 18` (15 <= value < 20).
 *
 * If not found, return (-1)
 */
let findLowerIndexInRange: (int, array(int)) => int =
  (value, array) => {
    let length = array |> Array.length;
    if (value >= array[length - 1]) {
      length - 1;
    } else {
      let rec loop = (min, max) => {
        let mid = min + (max - min) asr 1;

        if (value < array[mid]) {
          loop(min, mid - 1);
        } else if (value >= array[mid + 1]) {
          loop(mid + 1, max);
        } else {
          mid;
        };
      };
      loop(0, length - 2);
    };
  };

let toLoc = (lineStartOffsets, offset) => {
  let line = findLowerIndexInRange(offset, lineStartOffsets);
  {line, col: offset - lineStartOffsets[line]};
};
