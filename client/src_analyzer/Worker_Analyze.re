module Make = (ESig: Worker_Evaluator.EvaluatorSig) => {
  module Evaluator = Worker_Evaluator.Make(ESig);
  open Worker_Types;

  let emptyStringToOption =
    fun
    | "" => None
    | str => Some(str);

  let parseStderr = stderr =>
    stderr->(Belt.Option.map(stderr => stderr->Worker_ParseLocation.parse));

  let makeBlockResult = phraseContent => {
    let stderr = phraseContent->Evaluator.PhraseContent.stderrGet;

    {
      block_result: {
        blockResult_evaluate:
          phraseContent->Evaluator.PhraseContent.valueGet->emptyStringToOption,
        blockResult_stdout:
          phraseContent
          ->Evaluator.PhraseContent.stdoutGet
          ->emptyStringToOption,
        blockResult_stderr: stderr->emptyStringToOption->parseStderr,
      },
      block_loc:
        phraseContent
        ->Evaluator.PhraseContent.locGet
        ->Js.Nullable.toOption
        ->{
            let f = optLoc =>
              switch (optLoc) {
              | None => failwith("Unimplemented")
              | Some(js_loc) => js_loc
              };
            f;
          }
        ->Worker_Location_Utils.compilerLocToLoc,
    };
  };

  let execute: (. bool, string) => (list(blockData), bool) =
    (. reset, code) => {
      if (reset) {
        Evaluator.reset();
      };
      let result = Evaluator.execute(code);
      let length = Belt.Array.length(result);

      let rec loop = (i, acc) =>
        if (i < length) {
          let js_phraseResult = result[i];

          switch (js_phraseResult->Evaluator.kindGet) {
          | "Ok" =>
            loop(
              i + 1,
              [js_phraseResult->Evaluator.valueGet->makeBlockResult, ...acc],
            )
          | "Error" => (
              [js_phraseResult->Evaluator.valueGet->makeBlockResult, ...acc],
              true,
            )
          | kind => raise(Invalid_argument("Unknown phrase result " ++ kind))
          };
        } else {
          (acc->Belt.List.reverse, false);
        };
      loop(0, []);
    };

  let link: (. Editor_Types.lang, string, string) => linkResult =
    (. lang, name, code) => {
      let filename = name ++ Editor_Types.langToExtension(lang);

      Evaluator.insertModule(filename, code);
    };

  exception Not_Implemented;

  let linkMany:
    (. Editor_Types.lang, list(Editor_Types.Link.link)) =>
    list((Editor_Types.Link.link, linkResult)) =
    (. lang, links) => {
      open Editor_Types.Link;
      let rec loop = (links, acc) =>
        switch (links) {
        | [] => acc
        | [singleLink, ...rest] =>
          let (name, result) =
            switch (singleLink) {
            | Internal(internalLink) =>
              let {name, code} = internalLink;
              let result = link(. lang, name, code);
              (singleLink, result);
            | External(_) => raise(Not_Implemented)
            };

          let hasError = Belt.Result.isError(result);

          hasError ?
            [(name, result), ...acc] :
            loop(rest, [(name, result), ...acc]);
        };
      loop(links, []);
    };

  let executeMany:
    (. Editor_Types.lang, list((string, string))) =>
    list((string, list(blockData))) =
    (. lang, codeBlocks) => {
      /* Reset before evaluating several blocks */
      /* Evaluator.reset(); */

      switch (lang) {
      | Editor_Types.ML => Evaluator.mlSyntax()
      | Editor_Types.RE => Evaluator.reSyntax()
      };
      /*
       * Execute blocks in order
       * Stop when encountering error in a block
       */
      let rec loop = (blocks, acc) =>
        switch (blocks) {
        | [] => acc
        | [(id, code), ...rest] =>
          let (result, hasError) = execute(. false, code);

          hasError ?
            [(id, result), ...acc] : loop(rest, [(id, result), ...acc]);
        };
      loop(codeBlocks, []);
    };
};
