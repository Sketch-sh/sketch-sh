type t;

[@bs.val] external unsafeExecute : string => string = "evaluator.execute";

let error = [||];

%bs.raw
{|
  function proxy(context, method, message) {
    return function() {
      error.push(arguments[0]);
      method.apply(context, Array.prototype.slice.apply(arguments))
    }
  }

  console.error = proxy(console, console.error, 'Error:')
|};

let clearError: unit => unit = [%bs.raw {|
  function () {
    error = []
  }
|}];

let execute = code =>
  Belt.Result.(
    switch (unsafeExecute(code)) {
    | "" =>
      let message = error |> Js.Array.joinWith("\n");
      clearError();
      Error(message);
    | a => Ok(a)
    }
  );
