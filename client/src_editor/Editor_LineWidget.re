Modules.require("./Editor_LineWidget.css");

type lintWidgetType =
  | Error
  | Warning;
let lintTypToString =
  fun
  | Error => "error"
  | Warning => "warning";

open Webapi.Dom;

let createLintWidget = (modifier, html) => {
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

let createValueWidget = text => {
  let widget = document |> Document.createElement("pre");
  widget->Element.setClassName("widget__value");

  let textNode = document |> Document.createTextNode(text);
  widget |> Element.appendChild(textNode);
  widget;
};

let createStdoutWidget = text => {
  let widget = document |> Document.createElement("div");
  widget->Element.setClassName("widget__stdout");

  let textNode = document |> Document.createTextNode(text);
  widget |> Element.appendChild(textNode);
  widget;
};
