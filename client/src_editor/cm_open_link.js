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
