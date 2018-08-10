/*
 * Returns the coordinates to center a popup window in the viewport with
 * the provided width and height args.
 */
let getCenterCoordinates = (~width, ~height) => {
  open Webapi.Dom.Window;
  let window = Webapi.Dom.window;
  let top = window->screenY + (window->outerHeight - height) / 2;
  let left = window->screenX + (window->outerWidth - width) / 2;
  (top, left);
};

/* A helper for creating popup */
let openPopup = (~width=1024, ~height=640, url) => {
  let (top, left) = getCenterCoordinates(~width, ~height);
  let params = {j|width=$(width), height=$(height), top=$(top), left=$(left)|j};
  let window = Webapi.Dom.window;
  window
  ->(Webapi.Dom.Window.open_(~url, ~name="authWindow", ~features=params))
  ->ignore;
};
