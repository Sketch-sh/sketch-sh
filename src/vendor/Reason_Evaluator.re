type t;

[@bs.val] external unsafeExecute : string => string = "evaluator.execute";

let log = [||];

%bs.raw
{|
  function proxy(context, method) {
    return function() {
      log.push(arguments[0]);
      method.apply(context, Array.prototype.slice.apply(arguments))
    }
  }

  console.error = proxy(console, console.error)
  console.log = proxy(console, console.log)
  console.warning = proxy(console, console.warning)
|};

let clearLog: unit => unit = [%bs.raw {|
  function () {
    log = []
  }
|}];

let execute = code => {
  let result = unsafeExecute(code);
  let message = log |> Js.Array.joinWith("\n");
  clearLog();
  Belt.Result.(
    switch (result) {
    | "" => Error(message)
    | a => Ok(message == "" ? a : message ++ "\n" ++ a)
    }
  );
};
