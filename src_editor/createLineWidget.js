export const createErrorWidget = html => {
  const wrapper = document.createElement("div");
  wrapper.className = "widget__error widget__error-lg";

  const content = wrapper.appendChild(document.createElement("span"));
  content.className = "widget__error-content";
  content.innerHTML = html;

  return wrapper;
};

export const createValueWidget = text => {
  const wrapper = document.createElement("div");
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
