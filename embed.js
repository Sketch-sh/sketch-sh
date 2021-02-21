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

var url =
  document.location.hostname == "localhost"
    ? "http://localhost:3000"
    : "https://sketch.sh";

var Sketch = {
  defaultHeight: "200px",
  defaultLang: "RE",
  defaultAutoHeight: true,
  onLoad: function() {
    var elements = document.querySelectorAll("[data-sketch]");
    var length = elements.length;
    var i = 0;
    for (; i < length; i++) {
      var ele = elements[i];
      console.log(ele.dataset);
      this.createEmbed({
        element: ele,
        height: ele.dataset.height || this.defaultHeight,
        lang: ele.dataset.lang || this.defaultLang,
        autoHeight: ele.dataset.autoHeight || this.defaultAutoHeight,
        package: ele.dataset.package,
        source: ele.innerText,
      });
    }
  },
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
    var height = options.height || this.defaultHeight;
    var lang = options.lang || this.defaultLang;
    var autoHeight = options.autoHeight || this.defaultAutoHeight;
    var externalPackage = options.package || null;
    console.log(externalPackage);
    var iframe = document.createElement("iframe");
    iframe.setAttribute(
      "src",
      url +
        "/embed.html?value=" +
        btoa(source) +
        ("&lang=" + lang) +
        ("&autoHeight=" + autoHeight) +
        (externalPackage ? "&package=" + externalPackage : "")
    );
    iframe.width = "100%";
    iframe.height = height;
    iframe.style.border = "none";

    element.parentNode.replaceChild(iframe, element);
  },
};

onReady(Sketch.onLoad.bind(Sketch));

window.addEventListener("message", function(e) {
  if (e.origin !== url) return;

  var data = e.data;

  if (data.type !== "iframe.resize") {
    return false;
  }

  var iframe = document.querySelector('iframe[src="' + data.src + '"]');
  if (!iframe) {
    return false;
  }

  if (data.height) {
    iframe.height = data.height;
  }
});
