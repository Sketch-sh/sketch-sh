let red = (~label, message) =>
  Js.log3(
    {j|%c $label %c $message|j},
    "background-color: #f00; color: #fff; padding: 2px; font-weight: bold;",
    "background-color: #fcc; padding: 2px;",
  );

let green = (~label, message) =>
  Js.log3(
    {j|%c $label %c $message|j},
    "background-color: #0f0; color: #000; padding: 2px; font-weight: bold;",
    "background-color: #cfc; padding: 2px;",
  );

let blue = (~label, message) =>
  Js.log3(
    {j|%c $label %c $message|j},
    "background-color: #00f; color: #fff; padding: 2px; font-weight: bold;",
    "background-color: #ccf; padding: 2px;",
  );
