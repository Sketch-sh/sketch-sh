let str = ReasonReact.string;
let tapLog = a => {
  Js.log(a);
  a;
};

let valueFromEvent = evt : string => {
  let obj = evt |> ReactEventRe.Form.target |> ReactDOMRe.domElementToObj;
  obj##value;
};

let (=>>) = (value, render) =>
  switch (value) {
  | None => ReasonReact.null
  | Some(value) => render(value)
  };

let handleError = Js.Promise.(catch(error => Js.log(error) |. resolve));

let splitOnChar = (sep, s) => {
  let j = ref(String.length(s));
  let r = ref([]);
  for (i in String.length(s) - 1 downto 0) {
    if (String.unsafe_get(s, i) == sep) {
      r.contents = [
        String.sub(s, i + 1, j.contents - i - 1),
        ...r.contents,
      ];
      j.contents = i;
    };
  };
  let tl = r.contents;
  [String.sub(s, 0, j.contents), ...tl];
};

let listFindOpt = (p, l) =>
  try (Some(List.find(p, l))) {
  | Not_found => None
  };

let countLine = s => {
  let length = String.length(s);

  let rec countLine' = (i, result) =>
    if (i < length) {
      switch (s.[i]) {
      | '\n' => countLine'(i + 1, result + 1)
      | _ => countLine'(i + 1, result)
      };
    } else {
      result;
    };
  countLine'(0, 0);
};
