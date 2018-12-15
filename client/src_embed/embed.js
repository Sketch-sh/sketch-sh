"use strict";

var onReady = function(cb) {
  // https://plainjs.com/javascript/events/running-code-when-the-document-is-ready-15/
  // in case the document is already rendered
  if (document.readyState != "loading") cb();
  // modern browsers
  else if (document.addEventListener)
    document.addEventListener("DOMContentLoaded", cb);
  // IE <= 8
  else
    document.attachEvent("onreadystatechange", function() {
      if (document.readyState == "complete") cb();
    });
};

var Sketch = {
  createEmbed: function(options) {
    var options = options || {};
    if (!options.element) {
      throw new Error("options.element is required");
    } else {
      var element = options.element;
    }
    if (!options.source) {
      throw new Error("options.source is required");
    } else {
      var source = options.source;
    }
    var height = options.height || "200px";

    var iframe = document.createElement("iframe");
    iframe.setAttribute(
      "src",
      "http://localhost:3000/embed.html?value=" + btoa(source)
    );
    iframe.style.width = "100%";
    iframe.style.height = height;
    iframe.style.border = "none";

    element.parentNode.replaceChild(iframe, element);
  },
};
