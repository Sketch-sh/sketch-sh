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

const createErrorWidget = html => {
  const wrapper = document.createElement("div");
  wrapper.className = "widget__error widget__error-lg";

  const content = wrapper.appendChild(document.createElement("span"));
  content.className = "widget__error-content";
  content.innerHTML = html;

  return {
    wrapper,
    options: {
      coverGutter: true,
      noHScroll: false,
      above: false,
      showIfHidden: false,
    },
  };
};

const createValueWidget = text => {
  const wrapper = document.createElement("div");
  wrapper.className = "widget__value";
  wrapper.appendChild(document.createTextNode(text));

  return {
    wrapper,
    options: {
      coverGutter: false,
      noHScroll: true,
      above: false,
      showIfHidden: false,
    },
  };
};

const createStdoutWidget = text => {
  const wrapper = document.createElement("div");
  wrapper.className = "widget__stdout";
  wrapper.appendChild(document.createTextNode(text));

  return {
    wrapper,
    options: {
      coverGutter: false,
      noHScroll: false,
      above: false,
      showIfHidden: false,
    },
  };
};

class CodeMirror extends Component {
  constructor(props) {
    super(props);
    this.widgets = [];
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

  componentDidUpdate(prevProps) {
    let widgets = this.props.widgets;
    if (prevProps.widgets !== widgets) {
      this.widgets.forEach(domNode => {
        // TODO: should I remove the domNode ?
        // Potential memory leaks?
        domNode.clear();
      });

      let editor = this.editor;
      this.widgets = widgets.reduce((acc, w) => {
        let type = w.type;
        let wrapper = null;
        if (type === 1 /* Lw_Value */) {
          wrapper = createValueWidget(w.content);
        } else if (type === 0 /* Lw_Error */) {
          wrapper = createErrorWidget(w.content);
        } else if (type === 2 /* Lw_Stdout */) {
          wrapper = createStdoutWidget(w.content);
        } else {
          return acc;
        }

        let lineWidget = editor.addLineWidget(
          w.line,
          wrapper.wrapper,
          wrapper.options
        );
        acc.push(lineWidget);
        return acc;
      }, []);
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
        widgets={this.props.widgets}
        options={{
          mode: "rust",
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
            "Shift-Enter": cm => {
              this.props.onExecute();
              // let doc = cm.getDoc();
              // let getRange = doc.getRange(
              //   { line: 15, ch: 1 },
              //   { line: 16, ch: 34 }
              // );

              // const widget = createSignatureWidget(
              //   "let pi: float = 3.14159265358979312;"
              // );
              // {
              //   coverGutter: true,
              //   noHScroll: true,
              // }
              // console.log(cm.lineInfo(1));
              // cm.addWidget({ line: 0, ch: 1 }, widget);
            },
          },
        }}
      />
    );
  }
}

CodeBlock.defaultProps = {
  value: "",
};
