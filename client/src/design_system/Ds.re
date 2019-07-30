module Unit = Ds_unit;
module Button = Ds_button;

open Css;
let splitter_className =
  [unsafe("borderColor", "#E9ECEE !important")]->style;

let console_style = {
  "LOG_COLOR": Unit.Color.(dark->to_string),
  "LOG_BORDER": Unit.Color.(light_3->to_string),
  "BASE_FONT_SIZE": "14px",
};
