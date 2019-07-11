module type Sig = {
  type capture;
  let start: unit => capture;
  let stop: capture => string;
};
