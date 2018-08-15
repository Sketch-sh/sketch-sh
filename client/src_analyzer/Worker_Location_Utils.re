open Worker_Types;

let compilerPosToCodeEditorPos: Worker_Evaluator.Types.pos => pos =
  position =>
    Worker_Evaluator.Types.{
      line: position->lineGet - 1,
      col: position->colGet,
    };

let compilerLocToLoc:
  ((Worker_Evaluator.Types.pos, Worker_Evaluator.Types.pos)) => (pos, pos) =
  compilerPos => {
    let (from, to_) = compilerPos;
    (compilerPosToCodeEditorPos(from), compilerPosToCodeEditorPos(to_));
  };
