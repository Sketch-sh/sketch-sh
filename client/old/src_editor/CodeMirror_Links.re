[%%debugger.chrome];
open Utils;
open CodeMirror;

type extractedToken = {
  colStart: int,
  colEnd: int,
  line: int,
  content: string,
  withTitle: bool,
};

type collectorState =
  | Tracking(extractedToken)
  | NotTracking;

let linkCollector = (tokens: array(Token.t), ~line): list(extractedToken) => {
  let collect =
    tokens->Belt.Array.reduce((NotTracking, []), ((state, result), token) =>
      switch (token->Token.typGet->Js.Nullable.toOption) {
      | Some(typ) =>
        let splitted = typ |> Js.String.split(" ");
        if (inArray(splitted, "link") || inArray(splitted, "url")) {
          let string = token->Token.stringGet;

          let state =
            switch (state) {
            | NotTracking =>
              Tracking({
                content: string,
                colStart: token->Token.startGet,
                colEnd: token->Token.end_Get,
                line,
                withTitle: false,
              })
            | Tracking(data) =>
              let {content, colStart: _} = data;
              Tracking({
                ...data,
                content: content ++ string,
                colEnd: token->Token.end_Get,
                withTitle: inArray(splitted, "url"),
              });
            };
          (state, result);
        } else {
          switch (state) {
          | NotTracking => (state, result)
          | Tracking(data) => (NotTracking, [data, ...result])
          };
        };
      | _ =>
        switch (state) {
        | NotTracking => (state, result)
        | Tracking(data) => (NotTracking, [data, ...result])
        }
      }
    );
  let (state, collected) = collect;
  switch (state) {
  | NotTracking => collected
  | Tracking(data) => [data, ...collected]
  };
};

let getAllLinks = (cm, ~start, ~end_) => {
  let rec loop = (line, acc) =>
    if (line < end_) {
      let tokens = cm->(Editor.getLineTokens(~line, ~precise=true));
      let links = tokens->(linkCollector(~line));
      let res =
        switch (links) {
        | [] => acc
        | links => Belt.List.concat(acc, links)
        };
      loop(line + 1, res);
    } else {
      acc;
    };
  loop(start, []);
};

let icon = {|<svg viewBox="0 0 24 24">
  <path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6"></path>
  <polyline points="15 3 21 3 21 9"></polyline>
  <line x1="10" y1="14" x2="21" y2="3"></line>
</svg>|};

let urlRegex = [%re
  {|/^(http:\/\/www\.|https:\/\/www\.|http:\/\/|https:\/\/)?[a-z0-9]+([\-\.]{1}[a-z0-9]+)*\.[a-z]{2,5}(:[0-9]{1,5})?(\/.*)?$/|}
];
let withTitleRegex = [%re {|/\[([^\[\]]+)\]\(([^)]+)/|}];

let createLinkHandleNode = text => {
  open Webapi.Dom;
  let marker = document |> Document.createElement("span");
  marker->Element.setClassName("cm-link-marker cm-link");

  let textNode = document |> Document.createElement("span");
  textNode->Element.setInnerText(text);
  marker |> Element.appendChild(textNode);

  let widget = document |> Document.createElement("a");
  widget |> Element.setAttribute("href", text);
  widget |> Element.setAttribute("target", "_blank");
  widget |> Element.setAttribute("rel", "noopener noreferrer");
  widget->Element.setInnerHTML(icon);

  marker |> Element.appendChild(widget);
  (marker, textNode);
};

/*
 *  <span class="cm-link-marker">
 *    <span>
 *      <span class="cm-link">[title]</span>
 *      <span class="cm-string cm-url">(url)</span>
 *    </span>
 *    <a href=..>icon</a>
 *  </span>
 */
let createLinkHandleNodeWithTitle = (~href, ~title) => {
  open Webapi.Dom;
  let marker = document |> Document.createElement("span");
  marker->Element.setClassName("cm-link-marker");

  let textNode = document |> Document.createElement("span");
  textNode->Element.setInnerHTML(
    {j|
        <span class="cm-link">[$(title)]</span>
        <span class="cm-string cm-url">($(href))</span>
      |j},
  );
  marker |> Element.appendChild(textNode);

  let widget = document |> Document.createElement("a");
  widget |> Element.setAttribute("href", href);
  widget |> Element.setAttribute("target", "_blank");
  widget |> Element.setAttribute("rel", "noopener noreferrer");
  widget->Element.setInnerHTML(icon);

  marker |> Element.appendChild(widget);
  (marker, textNode);
};

let createLinkHandleNode = (text, withTitle) =>
  if (withTitle) {
    let split = text |> Js.String.splitByRe(withTitleRegex);

    if (split |> Array.length == 4) {
      let href = split[2]->Belt.Option.getWithDefault("");
      if (urlRegex->Js.Re.test_(href)) {
        Some(createLinkHandleNodeWithTitle(~href, ~title=split[1]));
      } else {
        None;
      };
    } else {
      None;
    };
  } else if (urlRegex->Js.Re.test_(text)) {
    Some(createLinkHandleNode(text));
  } else {
    None;
  };

/*
 * break text marker
 * put cursor to begining of marker
 * focus on editor
 */
let breakMark = (cm, marker) =>
  cm->Editor.operation((.) => {
    let pos = marker->TextMarker.find->TextMarker.fromGet;
    let doc = cm->Editor.getDoc;

    doc->Doc.setCursor(pos);
    cm->Editor.focus;
    marker->TextMarker.clear;
  });

let linkToMarker = (cm, doc, link) =>
  switch (createLinkHandleNode(link.content, link.withTitle)) {
  | None => None
  | Some((markerNode, textNode)) =>
    let marker =
      doc->(
             Doc.markText(
               ~from=Position.make(~line=link.line, ~ch=link.colStart, ()),
               ~to_=Position.make(~line=link.line, ~ch=link.colEnd, ()),
               ~option=
                 Doc.markTextOption(
                   ~clearOnEnter=true,
                   ~clearWhenEmpty=true,
                   ~handleMouseEvents=false,
                   ~replacedWith=markerNode,
                   (),
                 ),
             )
           );
    textNode
    |> Webapi.Dom.Element.addClickEventListener(_event =>
         breakMark(cm, marker)
       );
    Some(marker);
  };

let setMarkers = (cm, cachedMarkers) => {
  let doc = cm->Editor.getDoc;
  let lastLine = cm->Editor.lineCount;
  let allLinks = cm->(getAllLinks(~start=0, ~end_=lastLine));

  cachedMarkers :=
    allLinks->Belt.List.reduce([], (acc, link) =>
      switch (linkToMarker(cm, doc, link)) {
      | None => acc
      | Some(marker) => [marker, ...acc]
      }
    );
};

let register = cm => {
  let cachedMarkers = ref([]);

  /* This is for initial setValue */
  cm->Editor.onChange((cm, _) =>
    if (!cm->Editor.hasFocus) {
      setMarkers(cm, cachedMarkers);
    }
  );

  cm->Editor.onBlur((cm, _) => setMarkers(cm, cachedMarkers));

  cm->Editor.onFocus((cm, _) => {
    cm->Editor.operation((.) =>
      (cachedMarkers^)->Belt.List.forEach(marker => marker->TextMarker.clear)
    );
    cachedMarkers := [];
  });
};
