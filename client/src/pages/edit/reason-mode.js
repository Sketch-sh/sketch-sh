// CodeMirror, copyright (c) by Marijn Haverbeke and others
// Distributed under an MIT license: http://codemirror.net/LICENSE

(function(mod) {
  if (typeof exports == "object" && typeof module == "object")
    // CommonJS
    mod(
      require("codemirror/lib/codemirror"),
      require("codemirror/mode/clike/clike")
    );
  else if (typeof define == "function" && define.amd)
    // AMD
    define(["codemirror/lib/codemirror", "codemirror/mode/clike/clike"], mod);
  // Plain browser env
  else mod(CodeMirror);
})(function(CodeMirror) {
  "use strict";

  /* https://github.com/facebook/reason/blob/master/src/reason-parser/reason_lexer.mll#L94-L154 */
  var keywords = (
    "and as assert begin class constraint done downto exception external fun " +
    "esfun function functor include inherit initializer lazy let pub mutable new nonrec " +
    "object of open or pri rec then to type val virtual" +
    "try catch finally do else for if switch while import library export " +
    "part of show hide is as"
  ).split(" ");
  var blockKeywords = "try catch match with else for if switch while do begin end in module sig struct".split(
    " "
  );
  var atoms = "unit int char exn string int32 int64 float bool option mod land lor lxor lsl lsr asr".split(
    " "
  );
  var builtins = "true false Error Ok None Some".split(" ");

  function set(words) {
    var obj = {};
    for (var i = 0; i < words.length; ++i) obj[words[i]] = true;
    return obj;
  }

  CodeMirror.defineMIME("application/reason", {
    name: "clike",
    keywords: set(keywords),
    blockKeywords: set(blockKeywords),
    atoms: set(atoms),
    builtin: set(builtins),

    hooks: {
      "{": function(stream, state) {
        if (stream.eat("|")) {
          state.longString = true;
          state.tokenize = tokenLongString;
          return state.tokenize(stream, state);
        }
      },

      // array open [|
      "[": function(stream, state) {
        if (stream.eat("|")) {
          return "operator";
        }
        return null;
      },
      // array close |]
      "|": function(stream, state) {
        if (stream.eat("]")) {
          return "operator";
        }
        return "operator";
      },

      '"': function(stream, state) {
        state.tokenize = tokenString;
        return state.tokenize(stream, state);
      },

      "'": function(stream, state) {
        state.tokenize = tokenPolymorphicVar;
        return state.tokenize(stream, state);
      },

      "/": function(stream, state) {
        if (!stream.eat("*")) return false;
        state.tokenize = tokenNestedComment(1);
        return state.tokenize(stream, state);
      },
    },
  });

  function tokenPolymorphicVar(stream, state) {
    /* A char can't have more than 4 characters */
    var count = 0;
    var next;
    while ((next = stream.next()) != null) {
      // char
      if (stream.eat("'") && count <= 4) {
        state.tokenize = null;
        return "string";
        break;
      }
      // Polymorphic variable
      if (!/\w/.test(stream.peek())) {
        state.tokenize = null;
        return "variable-2";
        break;
      }
      count++;
    }
    state.tokenize = null;
    return null;
  }

  function tokenString(stream, state) {
    var next,
      end = false,
      escaped = false;
    while ((next = stream.next()) != null) {
      if (next === '"' && !escaped) {
        end = true;
        break;
      }
      escaped = !escaped && next === "\\";
    }
    if (end && !escaped) {
      state.tokenize = false;
    }
    return "string";
  }

  function tokenNestedComment(depth) {
    return function(stream, state) {
      var ch;
      while ((ch = stream.next())) {
        if (ch == "*" && stream.eat("/")) {
          if (depth == 1) {
            state.tokenize = null;
            break;
          } else {
            state.tokenize = tokenNestedComment(depth - 1);
            return state.tokenize(stream, state);
          }
        } else if (ch == "/" && stream.eat("*")) {
          state.tokenize = tokenNestedComment(depth + 1);
          return state.tokenize(stream, state);
        }
      }
      return "comment";
    };
  }

  function tokenLongString(stream, state) {
    var prev, next;
    while (state.longString && (next = stream.next()) != null) {
      if (prev === "|" && next === "}") state.longString = false;
      prev = next;
    }
    if (!state.longString) {
      state.tokenize = false;
    }
    return "string";
  }

  CodeMirror.registerHelper(
    "hintWords",
    "application/reason",
    keywords.concat(atoms).concat(builtins)
  );

  // This is needed to make loading through meta.js work.
  CodeMirror.defineMode(
    "reason",
    function(conf) {
      return Object.assign(CodeMirror.getMode(conf, "application/reason"), {
        lineComment: undefined, // reason doesn't have line comments
      });
    },
    "clike"
  );
});
