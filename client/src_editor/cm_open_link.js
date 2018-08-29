const linkHandler = e => {
  const shouldOpen = e.metaKey || e.ctrlKey;
  const target = e.target;
  const link = e.target.nextSibling;
  if (shouldOpen) {
    if (target.classList.contains("cm-link")) {
      if (link && link.classList && link.classList.contains("cm-url")) {
        const href = link.innerText.replace(/[()]/g, "");
        window.open(href);
      } else {
        window.open(target.innerText);
      }
      event.preventDefault();
    } else if (target.classList.contains("cm-url")) {
      const href = target.innerText.replace(/[()]/g, "");
      window.open(href);
    }
  }
};

export { linkHandler };

function hoverWidgetOnOverlay(cm, overlayClass, widget) {
  cm.addWidget({ line: 0, ch: 0 }, widget, true);
  widget.style.position = "fixed";
  widget.style.zIndex = 100000;
  widget.style.top = widget.style.left = "-1000px"; // hide it
  widget.dataset.token = null;

  cm.getWrapperElement().addEventListener("mousemove", e => {
    let onToken = e.target.classList.contains("cm-" + overlayClass),
      onWidget = e.target === widget || widget.contains(e.target);

    if (onToken && e.target.innerText !== widget.dataset.token) {
      // entered token, show widget
      var rect = e.target.getBoundingClientRect();
      widget.style.left = rect.right + "px";
      widget.style.top = rect.top + "px";

      widget.dataset.token = e.target.innerText;

      if (typeof widget.onShown === "function") widget.onShown();
    } else if (e.target === widget || widget.contains(e.target)) {
      // entered widget, call widget.onEntered
      if (
        widget.dataset.entered === "true" &&
        typeof widget.onEntered === "function"
      )
        widget.onEntered();
      widget.dataset.entered = "true";
    } else if (!onToken && widget.style.left !== "-1000px") {
      // we stepped outside
      widget.style.top = widget.style.left = "-1000px"; // hide it
      delete widget.dataset.token;
      widget.dataset.entered = "false";
      if (typeof widget.onHidden === "function") widget.onHidden();
    }

    return true;
  });
}

let widget = document.createElement("button");
widget.title = "Open link in new tab";
widget.className = "cm-link-widget";
widget.innerHTML = `
  <svg viewBox="0 0 24 24">
  <path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6"></path>
  <polyline points="15 3 21 3 21 9"></polyline>
  <line x1="10" y1="14" x2="21" y2="3"></line>
  </svg>`;

const urlRegex = /^(http:\/\/www\.|https:\/\/www\.|http:\/\/|https:\/\/)?[a-z0-9]+([\-\.]{1}[a-z0-9]+)*\.[a-z]{2,5}(:[0-9]{1,5})?(\/.*)?$/gm;
widget.onclick = function(e) {
  if (!widget.dataset.token) return;

  let link = widget.dataset.token.replace(/[()]/g, "");
  if (urlRegex.test(link)) {
    window.open(link, "_blank");
  }

  return true;
};

function register(cm) {
  hoverWidgetOnOverlay(cm, "url", widget);
}
export { register };
