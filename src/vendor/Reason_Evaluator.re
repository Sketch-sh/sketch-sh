type t;

[@bs.val] external unsafeExecute : string => string = "evaluator.execute";

let log = [||];
let shouldRecordLog = false;

%bs.raw
{|
  function proxy(context, method) {
    return function() {
      if (shouldRecordLog) {
        log.push(arguments[0]);
      }
      method.apply(context, Array.prototype.slice.apply(arguments))
    }
  }

  console.error = proxy(console, console.error)
  console.log = proxy(console, console.log)
  console.warning = proxy(console, console.warning)
|};

let startRecord: unit => unit = [%bs.raw
  {|
  function () {
    shouldRecordLog = true
    log = []
  }
|}
];

let stopRecord: unit => unit = [%bs.raw
  {|
  function () {
    shouldRecordLog = false
    log = []
  }
|}
];

let execute = code => {
  startRecord();
  let result = unsafeExecute(code);
  let message = log |> Js.Array.joinWith("\n");
  stopRecord();
  Belt.Result.(
    switch (result) {
    | "" => Error(message)
    | a => Ok(message == "" ? a : message ++ "\n" ++ a)
    }
  );
};
