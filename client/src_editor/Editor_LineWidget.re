Modules.require("./Editor_LineWidget.css");

type cb = unit => unit;

type lintWidgetType =
  | Error
  | Warning;
let lintTypToString =
  fun
  | Error => "error"
  | Warning => "warning";

let createLintWidget = (modifier, html) => {
  open Webapi.Dom;
  let widget = document |> Document.createElement("div");
  widget
  ->Element.setClassName(
      "widget__lint widget__lint-" ++ lintTypToString(modifier),
    );

  let wrapper = document |> Document.createElement("div");
  wrapper->Element.setClassName("widget__lint--wrapper");
  widget |> Element.appendChild(wrapper);

  let content = document |> Document.createElement("span");
  wrapper |> Element.appendChild(content);
  content->Element.setClassName("wrapper__link--content");
  content->Element.setInnerHTML(html);

  widget;
};

let createErrorWidget = createLintWidget(Error);
let createWarningWidget = createLintWidget(Warning);

let toggleFold = ele => {
  open Webapi.Dom;
  let classList = ele |> Element.classList;
  classList |> DomTokenList.toggle("widget__foldable--folded") |> ignore;
};

let excuteCallback = cb =>
  switch (cb^) {
  | None => ()
  | Some(cb) => cb()
  };

let createValueWidget = text => {
  let toggleFoldCallback: ref(option(cb)) = ref(None);
  open Webapi.Dom;
  let widget = document |> Document.createElement("div");
  widget->Element.setClassName("widget__foldable");

  let toggle = document |> Document.createElement("div");
  toggle
  ->Element.setClassName(
      "widget__foldable__gutter CodeMirror-guttermarker-subtle",
    );
  widget |> Element.appendChild(toggle);

  let div = document |> Document.createElement("div");
  div->Element.setClassName("widget__value pre");
  widget |> Element.appendChild(div);

  let split = text |> Js.String.splitAtMost("\n", ~limit=10);
  let countLine = Js.Array.length(split);

  if (countLine >= 10) {
    let widgetCL = widget |> Element.classList;
    widgetCL |> DomTokenList.add("widget__foldable--show");

    let firstTenLinesPos =
      split
      ->Belt.Array.reduce(
          0,
          (pos, line) => pos + Js.String.length(line) + 1,
        );
    let display = text |> Js.String.substring(~from=0, ~to_=firstTenLinesPos);
    let truncated = text |> Js.String.substringToEnd(~from=firstTenLinesPos);

    let displayNode = document |> Document.createElement("span");
    displayNode->Element.setInnerText(display);
    div |> Element.appendChild(displayNode);

    let buttonNode = document |> Document.createElement("button");
    buttonNode->Element.setClassName("widget__value__button");
    buttonNode->Element.setInnerText("<truncated...>");
    div |> Element.appendChild(buttonNode);

    let toggleEventListener = _event => {
      toggleFold(widget);
      excuteCallback(toggleFoldCallback);
    };
    toggle |> Element.addClickEventListener(toggleEventListener);
    buttonNode |> Element.addClickEventListener(toggleEventListener);

    let truncatedNode = document |> Document.createElement("span");
    truncatedNode->Element.setClassName("widget__value--truncated");
    truncatedNode->Element.setInnerText(truncated);
    div |> Element.appendChild(truncatedNode);
  } else {
    let textNode = document |> Document.createTextNode(text);
    div |> Element.appendChild(textNode);
  };

  let setCallback: (ref(option(cb)), cb) => unit =
    (variable, cb) => variable := Some(cb);
  (widget, setCallback(toggleFoldCallback));
};

let createStdoutWidget = text => {
  open Webapi.Dom;
  let widget = document |> Document.createElement("div");
  widget->Element.setClassName("widget__stdout");

  let textNode = document |> Document.createTextNode(text);
  widget |> Element.appendChild(textNode);
  widget;
};
