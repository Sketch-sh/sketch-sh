module BracketCounting = {
  type t = {
    roundo: int,
    roundc: int,
    curlyo: int,
    curlyc: int,
    squareo: int,
    squarec: int,
    doubleQuote: int,
    quote: int,
  };

  let initialState = {
    roundo: 0,
    roundc: 0,
    curlyo: 0,
    curlyc: 0,
    squareo: 0,
    squarec: 0,
    doubleQuote: 0,
    quote: 0,
  };

  type stringTyp = 
    | Single
    | Double
    | Block;

  let make = () => {
    let bracketState = ref(initialState);
    let prevChar = ref(None);
    let parsingStr: ref(option(stringTyp)) = ref(None);

    let loop = (char) => {
      let s = bracketState^;
      let shouldTerminal = switch (char) {
      /* 
        Handle nested parsing 
        {|  {| |}
      */
      | '|' when prevChar^ == Some('{') && parsingStr^ == None  => 
        parsingStr := Some(Block);
        false;
      | '}' when prevChar^ == Some('|') => 
        parsingStr := None;
        false;
      | '"' =>
        if (parsingStr^ == None) {
          parsingStr := Some(Double);
          ();
        };
        bracketState := {...s, doubleQuote: s.doubleQuote + 1};
        false;
      /* Normal brackets */
      | '(' =>
        bracketState := {...s, roundo: s.roundo + 1};
        false;
      | ')' =>
        bracketState := {...s, roundc: s.roundc + 1};
        false;
      | '{' =>
        bracketState := {...s, curlyo: s.curlyo + 1};
        false;
      | '}' =>
        bracketState := {...s, curlyc: s.curlyc + 1};
        false;
      | '[' =>
        bracketState := {...s, squareo: s.squareo + 1};
        false;
      | ']' =>
        bracketState := {...s, squarec: s.squarec + 1};
        false;
      | ';' =>
        if (s.roundo == s.roundc
        && s.curlyo == s.curlyc
        && s.squareo == s.squarec
        && s.quote land 1 == 0
        && s.doubleQuote land 1 == 0
        ) {
          bracketState := initialState;
          true
        }
        else {
          false
        }
      | _ => false
      };
      prevChar := Some(char);
      shouldTerminal
    };
    loop;
  };
};

type loc = {
  line: int,
  col: int,
  offset: int,
};

type pos = (loc, loc);

type result = {
  pos,
  code: string,
};

/* TODO: Add support for comment */
let exprSeperator = code => {
  let length = String.length(code);

  let exprs = ref([]);

  let bracketState = ref(BracketCounting.make());

  let rec exprSeperator' = (i, exprStartOffset, shouldCount) =>
    if (i < length) {
      switch (code.[i]) {
      | '\n' =>
        exprSeperator'(
          i + 1,
          shouldCount ? exprStartOffset : exprStartOffset + 1,
          shouldCount,
        )
      | ' ' =>
        exprSeperator'(
          i + 1,
          shouldCount ? exprStartOffset : exprStartOffset + 1,
          shouldCount,
        )
      | char =>
        if (shouldCount && bracketState^(char)) {
          bracketState := BracketCounting.make();
          exprs :=
            [
              String.sub(code, exprStartOffset, i - exprStartOffset + 1),
              ...exprs^,
            ];
          exprSeperator'(i + 1, i + 1, false);
        } else {
          exprSeperator'(i + 1, exprStartOffset, true);
        }
      };
    } else {
      exprs^ |. List.rev;
    };

  exprSeperator'(0, 0, false);
};
