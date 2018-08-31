open Editor_Types;

let logRefmtError = (code, error) => {
  Js.log2("Code:\n", code);
  Js.log2("Error:\n", error);
};

let refmtAsLangSibling = (blockId, code, toLang, callback) =>
  switch (toLang) {
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
             | Ok(re) => callback(re, blockId)
             | Error(error) => {
                 logRefmtError(code, error);
                 Notify.error(
                   "There was an error reformatting your ML code to RE, check the console for details",
                 );
               }
           );
           resolve();
         })
      |> catch(error => {
           logRefmtError(code, error);
           resolve(
             Notify.error(
               "There was an error reformatting your ML code to RE, check the console for details",
             ),
           );
         })
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
             | Ok(ml) => callback(ml, blockId)
             | Error(error) => {
                 logRefmtError(code, error);
                 Notify.error(
                   "There was an error reformatting your RE code to ML, check the console for details",
                 );
               }
           );
           resolve();
         })
      |> catch(error => {
           logRefmtError(code, error);
           resolve(
             Notify.error(
               "There was an error reformatting your RE code to ML, check the console for details",
             ),
           );
         })
    )
  };

let prettyPrintRe = (code, blockId, callback) =>
  Js.Promise.(
    Editor_Worker.reToRe(. code)
    |> then_(
         (result: Belt.Result.t(string, Worker_Evaluator.Types.Refmt.error)) => {
         result
         |> (
           fun
           | Ok(re) => callback(re, blockId)
           | Error(error) => {
               logRefmtError(code, error);
               Notify.error(
                 "There was an error pretty printing your code, check the console for details",
               );
             }
         );
         resolve();
       })
    |> catch(error => {
         logRefmtError(code, error);
         resolve(
           Notify.error(
             "There was an error pretty printing your code, check the console for details",
           ),
         );
       })
    |> ignore
  );
