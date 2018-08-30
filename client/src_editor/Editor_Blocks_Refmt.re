open Editor_Types;
open Editor_Blocks;

/* could not get the component to call this properly when living in this file  */
let refmtAsLangSibling = (code, lang, blockId, send) =>
  switch (lang) {
  | RE =>
    Js.Promise.(
      Editor_Worker.mlToRe(. code)
      |> then_(
           (
             result: Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error),
           ) => {
           result
           |> (
             fun
             | Ok(re) => send(Block_MapRefmtToBlocks(re, blockId))
             | Error(error) => Js.log(error)
           );
           resolve();
         })
      |> catch(error => resolve(Js.log(error)))
    )
  | ML =>
    Js.Promise.(
      Editor_Worker.reToMl(. code)
      |> then_(
           (
             result: Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error),
           ) => {
           result
           |> (
             fun
             | Ok(ml) => send(Block_MapRefmtToBlocks(ml, blockId))
             | Error(error) => Js.log(error)
           );
           resolve();
         })
      |> catch(error => resolve(Js.log(error)))
    )
  };

/*
     these can be used to minimize the unwanted transformations of long code blocks over time
     running the first transformation through this should put it in it's final state (I think)
 */
let mlToMlRefmt = code =>
  Js.Promise.(
    Editor_Worker.mlToMl(. code)
    |> then_(
         (result: Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error)) => {
         result
         |> (
           fun
           | Ok(ml) => Js.log(ml)
           | Error(error) => Js.log(error)
         );
         resolve();
       })
    |> catch(error => resolve(Js.log(error)))
    |> ignore
  );

let reToReRefmt = code =>
  Js.Promise.(
    Editor_Worker.mlToMl(. code)
    |> then_(
         (result: Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error)) => {
         result
         |> (
           fun
           | Ok(ml) => Js.log(ml)
           | Error(error) => Js.log(error)
         );
         resolve();
       })
    |> catch(error => resolve(Js.log(error)))
    |> ignore
  );
