let str = ReasonReact.string;
let valueFromEvent = evt : string => (
                                       evt
                                       |> ReactEventRe.Form.target
                                       |> ReactDOMRe.domElementToObj
                                     )##value;

let isClosed = s => {
  let s = String.trim(s);
  let length = String.length(s);
  if (length == 0) {
    false;
  } else {
    let rec loop = (~roundo, ~roundc, ~curlyo, ~curlyc, ~squareo, ~squarec, i) =>
      if (i < length) {
        switch (s.[i]) {
        | '(' =>
          loop(
            ~roundo=roundo + 1,
            ~roundc,
            ~curlyo,
            ~curlyc,
            ~squareo,
            ~squarec,
            i + 1,
          )
        | ')' =>
          loop(
            ~roundo,
            ~roundc=roundc + 1,
            ~curlyo,
            ~curlyc,
            ~squareo,
            ~squarec,
            i + 1,
          )
        | '{' =>
          loop(
            ~roundo,
            ~roundc,
            ~curlyo=curlyo + 1,
            ~curlyc,
            ~squareo,
            ~squarec,
            i + 1,
          )
        | '}' =>
          loop(
            ~roundo,
            ~roundc,
            ~curlyo,
            ~curlyc=curlyc + 1,
            ~squareo,
            ~squarec,
            i + 1,
          )
        | '[' =>
          loop(
            ~roundo,
            ~roundc,
            ~curlyo,
            ~curlyc,
            ~squareo=squareo + 1,
            ~squarec,
            i + 1,
          )
        | ']' =>
          loop(
            ~roundo,
            ~roundc,
            ~curlyo,
            ~curlyc,
            ~squareo,
            ~squarec=squarec + 1,
            i + 1,
          )
        | _ =>
          loop(~roundo, ~roundc, ~curlyo, ~curlyc, ~squareo, ~squarec, i + 1)
        };
      } else {
        roundo == roundc
        && curlyo == curlyc
        && squareo == squarec
        && s.[length - 1] == ';';
      };
    loop(
      ~roundo=0,
      ~roundc=0,
      ~curlyo=0,
      ~curlyc=0,
      ~squareo=0,
      ~squarec=0,
      0,
    );
  };
};
