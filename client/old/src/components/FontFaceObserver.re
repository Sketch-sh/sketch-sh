/*
 * Observe font face
 * Put <FontFaceObserver.Provider /> at any component level
 */
module FontFaceObserver = {
  type t;
  [@bs.new] [@bs.module] external make: string => t = "fontfaceobserver";
  [@bs.new] [@bs.module]
  external makeWithOptions: (string, Js.t('a)) => t = "fontfaceobserver";
  [@bs.send] external load: t => Js.Promise.t(unit) = "load";
};

module FontSession = {
  let fontSessionKey = "rtop:font-observer";
  open Dom.Storage;
  let isLoaded = () => sessionStorage |> getItem(fontSessionKey);
  let loaded = () => sessionStorage |> setItem(fontSessionKey, "loaded");
};

let setBodyClass = className =>
  Webapi.Dom.(
    switch (document |> Document.asHtmlDocument) {
    | None => ()
    | Some(html) =>
      switch (html->HtmlDocument.body) {
      | None => ()
      | Some(body) => body->Element.classList |> DomTokenList.add(className)
      }
    }
  );

module Provider = {
  let component = ReasonReact.reducerComponent("FontFaceObserver_Provider");

  let make:
    (~className: string=?, ~font: string=?, ReactHelpers.childless) =>
    ReasonReact.component(unit, 'a, unit) =
    (~className="fira-code", ~font="Fira Code", _children) => {
      ...component,
      didMount: _ =>
        switch (FontSession.isLoaded()) {
        | None =>
          Js.Promise.(
            Js.Promise.all([|
              FontFaceObserver.make(font)->FontFaceObserver.load,
              FontFaceObserver.makeWithOptions(font, {"weight": 400})
              ->FontFaceObserver.load,
              FontFaceObserver.makeWithOptions(font, {"weight": 700})
              ->FontFaceObserver.load,
            |])
            |> then_(_ =>
                 setBodyClass(className)->FontSession.loaded->resolve
               )
          )
          ->ignore
        | Some(_) => setBodyClass(className)->FontSession.loaded
        },
      render: _self => ReasonReact.null,
    };
};
