

import * as React from "react";
import * as ReactDom from "react-dom";
import * as Caml_builtin_exceptions from "bs-platform/lib/es6/caml_builtin_exceptions.js";

function renderToElementWithClassName(reactElement, className) {
  var elements = document.getElementsByClassName(className);
  if (elements.length !== 0) {
    ReactDom.render(reactElement, elements[0]);
    return /* () */0;
  } else {
    throw [
          Caml_builtin_exceptions.invalid_argument,
          "ReactDOMRe.renderToElementWithClassName: no element of class " + (className + " found in the HTML.")
        ];
  }
}

function renderToElementWithId(reactElement, id) {
  var match = document.getElementById(id);
  if (match == null) {
    throw [
          Caml_builtin_exceptions.invalid_argument,
          "ReactDOMRe.renderToElementWithId : no element of id " + (id + " found in the HTML.")
        ];
  } else {
    ReactDom.render(reactElement, match);
    return /* () */0;
  }
}

function hydrateToElementWithClassName(reactElement, className) {
  var elements = document.getElementsByClassName(className);
  if (elements.length !== 0) {
    ReactDom.hydrate(reactElement, elements[0]);
    return /* () */0;
  } else {
    throw [
          Caml_builtin_exceptions.invalid_argument,
          "ReactDOMRe.hydrateToElementWithClassName: no element of class " + (className + " found in the HTML.")
        ];
  }
}

function hydrateToElementWithId(reactElement, id) {
  var match = document.getElementById(id);
  if (match == null) {
    throw [
          Caml_builtin_exceptions.invalid_argument,
          "ReactDOMRe.hydrateToElementWithId : no element of id " + (id + " found in the HTML.")
        ];
  } else {
    ReactDom.hydrate(reactElement, match);
    return /* () */0;
  }
}

var Ref = /* module */[];

function createElementVariadic(domClassName, props, children) {
  var variadicArguments = /* array */[
      domClassName,
      props
    ].concat(children);
  return React.createElement.apply(null, variadicArguments);
}

function combine(a, b) {
  return Object.assign(Object.assign({ }, a), b);
}

function unsafeAddProp(style, property, value) {
  var dict = { };
  dict[property] = value;
  return combine(style, dict);
}

var Style = /* module */[
  /* combine */combine,
  /* unsafeAddProp */unsafeAddProp
];

export {
  renderToElementWithClassName ,
  renderToElementWithId ,
  hydrateToElementWithClassName ,
  hydrateToElementWithId ,
  Ref ,
  createElementVariadic ,
  Style ,
  
}
/* react Not a pure module */
