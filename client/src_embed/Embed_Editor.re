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
        <span className="footer-cell">
          <span> "Powered by "->str </span>
          <strong> "Sketch"->str </strong>
        </span>
        <span className="footer_spacing" />
        <button className="footer_run" onClick={_ => handleRun()}>
          <Fi.Play className="footer_run_icon" />
          "Run"->str
        </button>
      </footer>
    </>,
};
