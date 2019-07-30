type size = [ | `sm | `md | `lg | `xl | `xxl];
module Space = {
  let s0 = `rem(0);
  let s1 = `rem(0.25);
  let s2 = `rem(0.5);
  let s3 = `rem(0.75);
  let s4 = `rem(1.);
  let s5 = `rem(1.25);
  let s6 = `rem(1.5);
  let s8 = `rem(2);
  let s10 = `rem(2.5);
  let s12 = `rem(3.);
  let s16 = `rem(4.);
  let s20 = `rem(5.);
};

module Color = {
  let brand_dark3 = `hex("203D54");
  let brand_dark2 = `hex("1A4971");
  let brand_dark1 = `hex("2368A2");
  let brand = `hex("1874d5");
  let brand_light1 = `hex("63A2D8");
  let brand_light2 = `hex("AAD4F5");
  let brand_light3 = `hex("EFF8FF");

  let brand_invert = `hex("fff");

  let bg_default = `hex("fff");

  let dark = `hex("21252A");
  let dark_2 = `hex("343A40");
  let dark_3 = `hex("495057");
  let dark_4 = `hex("868E96");
  let dark_5 = `hex("ADB5BD");

  let light = `hex("CFD4DA");
  let light_2 = `hex("DEE2E6");
  let light_3 = `hex("E9ECEE");
  let light_4 = `hex("F1F3F5");
  let light_5 = `hex("F8F9FA");

  let to_string =
    fun
    | `hex(a) => "#" ++ a;
};

module BorderWidth = {
  let bw1 = `px(1);
  let bw2 = `px(2);
};
