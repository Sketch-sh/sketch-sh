type bracketCounting = {
  roundo: int,
  roundc: int,
  curlyo: int,
  curlyc: int,
  squareo: int,
  squarec: int,
  quote: int,
};

let shouldEvaluate = str => {
  let length = String.length(str);

  let rec loop = (s, i) =>
    if (i < length) {
      let newState =
        switch (str.[i]) {
        | '(' => {...s, roundo: s.roundo + 1}
        | ')' => {...s, roundc: s.roundc + 1}
        | '{' => {...s, curlyo: s.curlyo + 1}
        | '}' => {...s, curlyc: s.curlyc + 1}
        | '[' => {...s, squareo: s.squareo + 1}
        | ']' => {...s, squarec: s.squarec + 1}
        | '"' => {...s, quote: s.quote + 1}
        | _ => s
        };
      loop(newState, i + 1);
    } else {
      s.roundo <= s.roundc
      && s.curlyo <= s.curlyc
      && s.squareo <= s.squarec
      && s.quote
      land 1 == 0
      && str.[length - 1] == ';';
    };
  loop(
    {
      roundo: 0,
      roundc: 0,
      curlyo: 0,
      curlyc: 0,
      squareo: 0,
      squarec: 0,
      quote: 0,
    },
    0,
  );
};

/* Guard empty string */
let shouldEvaluate = str => {
  let str = String.trim(str);
  let length = String.length(str);
  if (length == 0) {
    false;
  } else {
    shouldEvaluate(str);
  };
};
