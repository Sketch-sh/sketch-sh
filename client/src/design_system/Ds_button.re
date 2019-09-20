type state =
  | Disabled
  | Normal
  | Loading;

type color =
  | Secondary
  | Text
  | Brand;

module Styles = {
  open Css;
  open Ds_unit;

  let colorToCssValue =
    fun
    | (false, Brand) => [
        borderColor(Color.brand),
        background(Color.brand_invert),
        color(Color.brand),
      ]
    | (true, Brand) => [
        borderColor(Color.brand),
        background(Color.brand),
        color(Color.brand_invert),
        focus([boxShadow(~spread=`rem(0.2), `rgba((0, 123, 255, 0.25)))]),
      ]
    | (_, Text) => [
        borderColor(Color.dark_3),
        color(Color.dark_3),
        backgroundColor(Color.bg_default),
      ]
    | (_, Secondary) => [
        borderColor(Color.dark_4),
        color(Color.dark_4),
        backgroundColor(Color.bg_default),
      ];

  let size_to_pad = size => {
    let (v, h) =
      switch (size) {
      | `sm => Space.(s1, s2)
      | `md => Space.(s2, s4)
      | `lg
      | `xl
      | `xxl => Space.(s3, s4)
      };
    padding2(~v, ~h);
  };

  let size_to_font_weight =
    fun
    | `sm => `normal
    | `md
    | `lg
    | `xl
    | `xxl => `bold;

  let size_to_font_weight = size => size->size_to_font_weight->fontWeight;

  let button = (primary, btnColor, size) => {
    [|
      [
        size_to_pad(size),
        size_to_font_weight(size),
        borderRadius(`px(5)),
        borderWidth(BorderWidth.bw1),
        borderStyle(`solid),
        cursor(`pointer),
      ],
      colorToCssValue((primary, btnColor)),
    |]
    ->List.concatMany
    ->style;
  };
};

[@genType]
[@react.component]
let make =
    (
      ~color=Text,
      ~size=`md,
      ~state=Normal,
      ~primary=false,
      ~onClick=?,
      ~disabled=?,
      ~children,
    ) => {
  <button ?onClick ?disabled className={Styles.button(primary, color, size)}>
    children
  </button>;
};
