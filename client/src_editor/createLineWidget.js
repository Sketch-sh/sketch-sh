import "./createLineWidget.css";

const createLintWidget = (modifier, html) => {
  const widget = document.createElement("div");
  widget.className = "widget__lint widget__lint-" + modifier;

  const wrapper = widget.appendChild(document.createElement("div"));
  wrapper.className = "widget__lint--wrapper";

  const content = wrapper.appendChild(document.createElement("span"));
  content.className = "wrapper__link--content";
  content.innerHTML = html;

  return widget;
};

export const createErrorWidget = html => createLintWidget("error", html);
export const createWarningWidget = html => createLintWidget("warning", html);

export const createValueWidget = text => {
  const wrapper = document.createElement("pre");
  wrapper.className = "widget__value";
  wrapper.appendChild(document.createTextNode(text));

  return wrapper;
};

export const createStdoutWidget = text => {
  const wrapper = document.createElement("div");
  wrapper.className = "widget__stdout";
  wrapper.appendChild(document.createTextNode(text));

  return wrapper;
};
