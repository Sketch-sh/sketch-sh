open Utils;

module Transformer: {
  let format: (~formatPattern: string=?, string) => string;
  let relative: (~formatPattern: string=?, string) => string;
} = {
  let format = (~formatPattern="YYYY-MM-DD", date) =>
    date |> Js.Date.fromString |> DateFns.format(formatPattern);
  let now = Js.Date.make();
  let lastMonth = DateFns.subMonths(1., now);
  let relative = (~formatPattern="YYYY-MM-DD", date) => {
    let formatingDate = date |> Js.Date.fromString;
    let compareResult = DateFns.compareAsc(lastMonth, formatingDate);
    if (compareResult == 1) {
      formatingDate |> DateFns.format(formatPattern);
    } else {
      DateFns.distanceInWordsToNow(~addSuffix=true, formatingDate);
    };
  };
};

[@react.component]
let make = (~date: Js.Json.t, ~transformer=Transformer.relative, ~className=?) => {
  let date = date |> Js.Json.decodeString;
  switch (date) {
  | None => React.null
  | Some(date) =>
    <time dateTime=date ?className> {date->transformer->str} </time>
  };
};
