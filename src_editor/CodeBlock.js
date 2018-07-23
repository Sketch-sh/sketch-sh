// @ts-check
import React, { Component, PureComponent } from "react";
import codemirror from "codemirror";

import "codemirror/mode/mllike/mllike";
import "codemirror/addon/edit/matchbrackets";
import "codemirror/mode/rust/rust";
import "codemirror/lib/codemirror.css";

import "./cm-theme.css";
import "./source-editor.css";
import "./app.css";

class CodeMirror extends Component {
  constructor(props) {
    super(props);
  }

  componentDidMount() {
    this.editor = codemirror(this.div, this.props.options);
    this.editor.setValue(this.props.value);

    this.editor.on("change", (cm, metadata) => {
      const value = this.editor.getValue();
      if (value !== this.props.value && this.props.onChange) {
        this.props.onChange(value);
      }
    });
  }

  componentWillReceiveProps(nextProps) {
    if (
      this.props.value !== nextProps.value &&
      nextProps.value !== this.editor.getValue()
    ) {
      this.editor.setValue(nextProps.value);
    }
    if (this.props.firstLineNumber !== nextProps.firstLineNumber) {
      this.editor.setOption("firstLineNumber", nextProps.firstLineNumber);
    }
  }

  render() {
    return (
      <div className={this.props.className} ref={div => (this.div = div)} />
    );
  }
}

export { CodeMirror };

export default class CodeBlock extends PureComponent {
  render() {
    return (
      <CodeMirror
        value={this.props.value}
        onChange={this.props.onChange}
        firstLineNumber={this.props.firstLineNumber}
        options={{
          mode: "mllike",
          theme: "runkit-light",
          lineNumbers: true,
          viewportMargin: Infinity,
          matchBrackets: true,
          firstLineNumber: this.props.firstLineNumber,
          extraKeys: {
            Tab: function(cm) {
              var spaces = Array(cm.getOption("indentUnit") + 1).join(" ");
              cm.replaceSelection(spaces);
            },
            "Shift-Enter": function(cm) {
              const wrapper = document.createElement("div");
              wrapper.className =
                "syntax-error__wrapper syntax-error__wrapper-lg";

              const icon1 = wrapper.appendChild(document.createElement("div"));
              icon1.className = "syntax-error__icon syntax-error__icon-1";

              const icon2 = wrapper.appendChild(document.createElement("div"));
              icon2.className = "syntax-error__icon syntax-error__icon-2";

              const content = wrapper.appendChild(
                document.createElement("span")
              );
              content.className = "syntax-error__content";
              content.innerHTML = "syntax error";

              cm.addLineWidget(2, wrapper, {
                coverGutter: true,
                noHScroll: true,
              });
              console.log("execute code");
            },
          },
        }}
      />
    );
  }
}
