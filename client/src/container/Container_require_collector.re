// TODO: move this into web worker
// All the code below is taken from: https://github.com/facebook/jest/blob/master/packages/jest-haste-map/src/lib/dependencyExtractor.ts

// Negative look behind is not supported in Firefox nor in Safari
let notADotRegExp =
  try (Js.Re.fromString("(?<!\\.\\s*)")) {
  | _ => Js.Re.fromString("(?:^|[^.]\\s*)")
  };

let blockCommentRegExp = [%re {|/\/\*[^]*?\*\//g|}];
let lineCommentRegExp = [%re {|/\/\/.*/g|}];

let importOrExportRegExp = [%re
  {|/\b(?:import|export)\s+(?!type(?:of)?\s+)(?:[^'\"]+\s+from\s+)?([`'\"])([^'\"`]*?)(?:\1)/g|}
];

let requireOrDynamicImportRegExp =
  Js.Re.fromStringWithFlags(
    Js.Re.source(notADotRegExp)
    ++ {|\b(?:require|import)\s*\(\s*([`'"])([^'"`]*?)(?:\1)\s*(:?,\s*)?\)|},
    ~flags="g",
  );

let parse = code => {
  let dependencies = Belt.MutableSet.String.make();

  let addDependency = (match, _, dep) => {
    Belt.MutableSet.String.add(dependencies, dep);
    match;
  };

  let _ =
    code
    |> Js.String.replaceByRe(blockCommentRegExp, "")
    |> Js.String.replaceByRe(lineCommentRegExp, "")
    |> Js.String.unsafeReplaceBy0(importOrExportRegExp, addDependency)
    |> Js.String.unsafeReplaceBy0(requireOrDynamicImportRegExp, addDependency);

  Belt.MutableSet.String.toArray(dependencies);
};

/* Below is probably a safer alternative since it does not use the unsafeReplaceBy0 function.
      However, the "unsafe" code above is directly taken from the Jest repo which I guess we can be pretty
      confident about. I still keep the other commented out in case of.
   */

/*let rec extract = (str, regexp, set) => {
    switch (Js.Re.exec_(regexp, str)) {
    | None => set
    | Some(result) =>
      switch (Belt.Array.get(Js.Re.captures(result), 2)) {
      | None => extract(str, regexp, set)
      | Some(valueOrNull) =>
        switch (Js.Nullable.toOption(valueOrNull)) {
        | None => extract(str, regexp, set)
        | Some(value) => extract(str, regexp, Belt.Set.String.add(set, value))
        }
      }
    };
  };

  let parse = code => {
    let stripped =
      Js.String.replaceByRe(blockCommentRegExp, "", code)
      |> Js.String.replaceByRe(lineCommentRegExp, "");

    let extractFromCode = extract(stripped);

    let res =
      Belt.Set.String.empty
      |> extractFromCode(importOrExportRegExp)
      |> extractFromCode(requireOrDynamicImportRegExp);

    Belt.Set.String.toArray(res);
  };*/
