let str = ReasonReact.string;

let valueFromEvent = evt : string => {
  let obj = evt |> ReactEventRe.Form.target |> ReactDOMRe.domElementToObj;
  obj##value;
};

let (=>>) = (value, render) =>
  switch (value) {
  | None => ReasonReact.null
  | Some(value) => render(value)
  };
