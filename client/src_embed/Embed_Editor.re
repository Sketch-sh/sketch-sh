open Utils;

let component = ReasonReact.statelessComponent("Embed_Editor");

let make = (~value, ~handleValueChange, ~handleRun, ~widgets, _children) => {
  ...component,
  render: _self =>
    <>
      <section id="editor">
        <Editor_CodeBlock
          value
          firstLineNumber=1
          focused={Some(FcTyp_EditorFocus)}
          widgets
          viewportMargin=10.
          onChange={(value, _) => handleValueChange(value)}
        />
      </section>
      <footer id="footer">
        <a
          href="https://sketch.sh"
          target="_blank"
          rel="noopener noreferrer"
          className="footer-cell footer-powered-by">
          <span> "Powered by "->str </span>
          <strong> "Sketch"->str </strong>
        </a>
        <span className="footer_spacing" />
        <button className="footer_run" onClick={_ => handleRun()}>
          <Fi.Play className="footer_run_icon" />
          "Run"->str
        </button>
      </footer>
    </>,
};
