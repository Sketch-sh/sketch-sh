type editor;

module EditorConfiguration = {
  [@bs.deriving abstract]
  type editorConfiguration = {
    /** string| The starting value of the editor. Can be a string, or a document object. */
    [@bs.optional]
    value: string,
    /** string|object. The mode to use. When not given, this will default to the first mode that was loaded.
        It may be a string, which either simply names the mode or is a MIME type associated with the mode.
        Alternatively, it may be an object containing configuration options for the mode,
        with a name property that names the mode (for example {name: "javascript", json: true}). */
    [@bs.optional]
    mode: string,
    /** The theme to style the editor with. You must make sure the CSS file defining the corresponding .cm-s-[name] styles is loaded.
        The default is "default". */
    [@bs.optional]
    theme: string,
    /** How many spaces a block (whatever that means in the edited language) should be indented. The default is 2. */
    [@bs.optional]
    indentUnit: int,
    /** Whether to use the context-sensitive indentation that the mode provides (or just indent the same as the line before). Defaults to true. */
    [@bs.optional]
    smartIndent: bool,
    /** The width of a tab character. Defaults to 4. */
    [@bs.optional]
    tabSize: int,
    /** Whether, when indenting, the first N*tabSize spaces should be replaced by N tabs. Default is false. */
    [@bs.optional]
    indentWithTabs: bool,
    /** Configures whether the editor should re-indent the current line when a character is typed
        that might change its proper indentation (only works if the mode supports indentation). Default is true. */
    [@bs.optional]
    electricChars: bool,
    /** Determines whether horizontal cursor movement through right-to-left (Arabic, Hebrew) text
        is visual (pressing the left arrow moves the cursor left)
        or logical (pressing the left arrow moves to the next lower index in the string, which is visually right in right-to-left text).
        The default is false on Windows, and true on other platforms. */
    [@bs.optional]
    rtlMoveVisually: bool,
    /** Configures the keymap to use. The default is "default", which is the only keymap defined in codemirror.js itself.
        Extra keymaps are found in the keymap directory. See the section on keymaps for more information. */
    [@bs.optional]
    keyMap: string,
    /** Can be used to specify extra keybindings for the editor, alongside the ones defined by keyMap. Should be either null, or a valid keymap value. */
    [@bs.optional]
    extraKeys: Js.Dict.t(string),
    /** Whether CodeMirror should scroll or wrap for long lines. Defaults to false (scroll). */
    [@bs.optional]
    lineWrapping: bool,
    /** Whether to show line numbers to the left of the editor. */
    [@bs.optional]
    lineNumbers: bool,
    /** At which number to start counting lines. Default is 1. */
    [@bs.optional]
    firstLineNumber: int,
    /** A function used to format line numbers. The function is passed the line number, and should return a string that will be shown in the gutter. */
    [@bs.optional]
    lineNumberFormatter: int => string,
    /** Can be used to add extra gutters (beyond or instead of the line number gutter).
        Should be an array of CSS class names, each of which defines a width (and optionally a background),
        and which will be used to draw the background of the gutters.
        May include the CodeMirror-linenumbers class, in order to explicitly set the position of the line number gutter
        (it will default to be to the right of all other gutters). These class names are the keys passed to setGutterMarker. */
    [@bs.optional]
    gutters: array(string),
    /** Provides an option foldGutter, which can be used to create a gutter with markers indicating the blocks that can be folded. */
    [@bs.optional]
    foldGutter: bool,
    /** Determines whether the gutter scrolls along with the content horizontally (false)
        or whether it stays fixed during horizontal scrolling (true, the default). */
    [@bs.optional]
    fixedGutter: bool,
    /**
    * Chooses a scrollbar implementation. The default is "native", showing native scrollbars. The core library also
    * provides the "null" style, which completely hides the scrollbars. Addons can implement additional scrollbar models.
    */
    [@bs.optional]
    scrollbarStyle: string,
    /** boolean|string. This disables editing of the editor content by the user. If the special value "nocursor" is given (instead of simply true), focusing of the editor is also disallowed. */
    [@bs.optional]
    /* TODO */
    readOnly: bool,
    /**Whether the cursor should be drawn when a selection is active. Defaults to false. */
    [@bs.optional]
    showCursorWhenSelecting: bool,
    /** The maximum number of undo levels that the editor stores. Defaults to 40. */
    [@bs.optional]
    undoDepth: int,
    /** The period of inactivity (in milliseconds) that will cause a new history event to be started when typing or deleting. Defaults to 500. */
    [@bs.optional]
    historyEventDelay: int,
    /** The tab index to assign to the editor. If not given, no tab index will be assigned. */
    [@bs.optional]
    tabindex: int,
    /** Can be used to make CodeMirror focus itself on initialization. Defaults to off.
        When fromTextArea is used, and no explicit value is given for this option, it will be set to true when either the source textarea is focused,
        or it has an autofocus attribute and no other element is focused. */
    [@bs.optional]
    autofocus: bool,
    /** Controls whether drag-and - drop is enabled. On by default. */
    [@bs.optional]
    dragDrop: bool,
    /** When given , this will be called when the editor is handling a dragenter , dragover , or drop event.
        It will be passed the editor instance and the event object as arguments.
        The callback can choose to handle the event itself , in which case it should return true to indicate that CodeMirror should not do anything further. */ /* TODO */ /* onDragEvent?: (instance: CodeMirror.Editor, event: Event) => boolean; *//** This provides a rather low - level hook into CodeMirror's key handling.
        If provided, this function will be called on every keydown, keyup, and keypress event that CodeMirror captures.
        It will be passed two arguments, the editor instance and the key event.
        This key event is pretty much the raw key event, except that a stop() method is always added to it.
        You could feed it to, for example, jQuery.Event to further normalize it.
        This function can inspect the key event, and handle it if it wants to.
        It may return true to tell CodeMirror to ignore the event.
        Be wary that, on some browsers, stopping a keydown does not stop the keypress from firing, whereas on others it does.
        If you respond to an event, you should probably inspect its type property and only do something when it is keydown
        (or keypress for actions that need character data). *//* TODO */ /* onKeyEvent?: (instance: CodeMirror.Editor, event: Event) => boolean; */ /** Half - period in milliseconds used for cursor blinking. The default blink rate is 530ms. */
    [@bs.optional]
    cursorBlinkRate: int,
    /** Determines the height of the cursor. Default is 1 , meaning it spans the whole height of the line.
        For some fonts (and by some tastes) a smaller height (for example 0.85),
        which causes the cursor to not reach all the way to the bottom of the line, looks better */
    [@bs.optional]
    cursorHeight: int,
    /** Highlighting is done by a pseudo background - thread that will work for workTime milliseconds,
        and then use timeout to sleep for workDelay milliseconds.
        The defaults are 200 and 300, you can change these options to make the highlighting more or less aggressive. */
    [@bs.optional]
    workTime: int,
    /** See workTime. */
    [@bs.optional]
    workDelay: int,
    /** Indicates how quickly CodeMirror should poll its input textarea for changes(when focused).
        Most input is captured by events, but some things, like IME input on some browsers, don't generate events that allow CodeMirror to properly detect it.
        Thus, it polls. Default is 100 milliseconds. */
    [@bs.optional]
    pollInterval: int,
    /** By default, CodeMirror will combine adjacent tokens into a single span if they have the same class.
        This will result in a simpler DOM tree, and thus perform better. With some kinds of styling(such as rounded corners),
        this will change the way the document looks. You can set this option to false to disable this behavior. */
    [@bs.optional]
    flattenSpans: bool,
    /** When highlighting long lines, in order to stay responsive, the editor will give up and simply style
        the rest of the line as plain text when it reaches a certain position. The default is 10000.
        You can set this to Infinity to turn off this behavior. */
    [@bs.optional]
    maxHighlightLength: int,
    /** Specifies the amount of lines that are rendered above and below the part of the document that's currently scrolled into view.
        This affects the amount of updates needed when scrolling, and the amount of work that such an update does.
        You should usually leave it at its default, 10. Can be set to Infinity to make sure the whole document is always rendered,
        and thus the browser's text search works on it. This will have bad effects on performance of big documents. */
    [@bs.optional]
    viewportMargin: float,
    /** Optional value to be used in conjunction with CodeMirror’s placeholder add-on. */
    [@bs.optional]
    placeholder: string,
    [@bs.optional]
    matchBrackets: bool,
    [@bs.optional]
    autoCloseBrackets: bool,
    [@bs.optional]
    styleActiveLine: bool,
  };

  type t = editorConfiguration;
  let make = editorConfiguration;
};

[@bs.module]
external make: (Dom.element, EditorConfiguration.t) => editor = "codemirror";

module Position = {
  [@bs.deriving abstract]
  type t = {
    ch: int,
    line: int,
    [@bs.optional]
    sticky: Js.Nullable.t(string),
  };
  let make = t;
};

module LineWidget = {
  type t;
  /**
    Removes the widget.
  */
  [@bs.send]
  external clear: t => unit = "clear";

  /** Call this if you made some change to the widget's DOM node that might affect its height.
  It'll force CodeMirror to update the height of the line that contains the widget. */
  [@bs.send]
  external changed: t => unit = "";
  [@bs.deriving abstract]
  type options = {
    /** Whether the widget should cover the gutter. */
    coverGutter: bool,
    /** Whether the widget should stay fixed in the face of horizontal scrolling. */
    noHScroll: bool,
    /** Causes the widget to be placed above instead of below the text of the line. */
    above: bool,
    /** When true, will cause the widget to be rendered even if the line it is associated with is hidden. */
    showIfHidden: bool,
    handleMouseEvents: bool,
  };
};

module EditorChange = {
  [@bs.deriving abstract]
  type t = {
    /** Position (in the pre-change coordinate system) where the change started.*/
    from: Position.t,
    /** Position (in the pre-change coordinate system) where the change ended. */
    [@bs.as "to"]
    to_: Position.t,
    /** Array of strings representing the text that replaced the changed range (split by line). */
    text: array(string),
    /**  Text that used to be between from and to, which is overwritten by this change. */
    removed: array(string),
    /**  String representing the origin of the change event and wether it can be merged with history */
    origin: string,
  };
};

module TextMarker = {
  type t;

  [@bs.send] external clear: t => unit = "";

  [@bs.deriving abstract]
  type findResult = {
    from: Position.t,
    [@bs.as "to"]
    to_: Position.t,
  };
  [@bs.send] external find: t => findResult = "";
};

module Doc = {
  type t;

  /** start is a an optional string indicating which end of the selection to return.
    It may be "start" , "end" , "head"(the side of the selection that moves when you press shift + arrow),
    or "anchor"(the fixed side of the selection).
    Omitting the argument is the same as passing "head".A { line , ch } object will be returned.
  */
  [@bs.send]
  external getCursor:
    (t, [@bs.string] [ | `start | [@bs.as "end"] `end_ | `head | `anchor]) =>
    Position.t =
    "";

  [@bs.send] external setCursor: (t, Position.t) => unit = "";

  [@bs.send] external clearHistory: t => unit = "";

  [@bs.deriving abstract]
  type markTextOption = {
    [@bs.optional]
    className: string,
    [@bs.optional]
    inclusiveLeft: bool,
    [@bs.optional]
    inclusiveRight: bool,
    [@bs.optional]
    atomic: bool,
    [@bs.optional]
    collapsed: bool,
    [@bs.optional]
    clearOnEnter: bool,
    [@bs.optional]
    clearWhenEmpty: bool,
    [@bs.optional]
    replacedWith: Dom.element,
    [@bs.optional]
    handleMouseEvents: bool,
    [@bs.optional]
    readOnly: bool,
    [@bs.optional]
    addToHistory: bool,
    [@bs.optional]
    startStyle: string,
    [@bs.optional]
    endStyle: string,
    [@bs.optional]
    css: string,
    [@bs.optional]
    title: string,
    [@bs.optional]
    shared: bool,
  };
  [@bs.send]
  external markText:
    (t, ~from: Position.t, ~to_: Position.t, ~option: markTextOption) =>
    TextMarker.t =
    "";
};

module Token = {
  [@bs.deriving abstract]
  type t = {
    start: int,
    [@bs.as "end"]
    end_: int,
    string,
    [@bs.as "type"]
    typ: Js.Nullable.t(string),
    state: Js.Json.t,
  };
};

module Editor = {
  [@bs.send] external getValue: editor => string = "";
  [@bs.send] external setValue: (editor, string) => unit = "";
  [@bs.send] external setOption: (editor, string, 'a) => unit = "";
  [@bs.send] external getOption: (editor, string) => 'a = "";
  [@bs.send] external getWrapperElement: editor => Dom.element = "";
  [@bs.send] external operation: (editor, (. unit) => 'a) => 'a = "";
  [@bs.send]
  external getLineTokens:
    (editor, ~line: int, ~precise: bool) => array(Token.t) =
    "";

  module GetOption = {
    [@bs.send]
    external indentUnit: (editor, [@bs.as "indentUnit"] _) => int =
      "getOption";
  };

  [@bs.send] external replaceSelection: (editor, string) => unit = "";
  [@bs.send]
  external addLineWidget:
    (
      editor,
      ~line: int,
      ~element: Dom.element,
      ~options: LineWidget.options
    ) =>
    LineWidget.t =
    "";

  [@bs.send] external getDoc: editor => Doc.t = "";
  [@bs.send] external hasFocus: editor => bool = "";
  [@bs.send] external focus: editor => unit = "";

  [@bs.send] external getLine: (editor, int) => string = "";
  [@bs.send] external lineCount: editor => int = "";

  [@bs.send]
  external onChange:
    (editor, [@bs.as "change"] _, (editor, EditorChange.t) => unit) => unit =
    "on";

  [@bs.send]
  external onFocus:
    (editor, [@bs.as "focus"] _, (editor, Dom.event) => unit) => unit =
    "on";

  [@bs.send]
  external onBlur:
    (editor, [@bs.as "blur"] _, (editor, Dom.event) => unit) => unit =
    "on";

  [@bs.send]
  external onKeydown:
    (editor, [@bs.as "keydown"] _, (editor, Dom.keyboardEvent) => unit) => unit =
    "on";
  [@bs.send]
  external onViewportChange:
    (editor, [@bs.as "viewportChange"] _, (editor, float, float) => unit) =>
    unit =
    "on";
  [@bs.send]
  external onUpdate: (editor, [@bs.as "update"] _, editor => unit) => unit =
    "on";
  [@bs.send]
  external onRefresh: (editor, [@bs.as "refresh"] _, editor => unit) => unit =
    "on";

  /* comment addon */
  [@bs.deriving abstract]
  type commentOptions = {
    [@bs.optional]
    padding: string,
    /* A string that will be inserted after opening and leading markers, and before closing comment markers. Defaults to a single space. */
    [@bs.optional]
    commentBlankLines: bool,
    /* Whether, when adding line comments, to also comment lines that contain only whitespace. */
    [@bs.optional]
    indent: bool,
    /* When adding line comments and this is turned on, it will align the comment block to the current indentation of the first line of the block. */
    [@bs.optional]
    fullLines: bool,
    /* When block commenting, this controls whether the whole lines are indented, or only the precise range that is given. Defaults to true. */
  };

  [@bs.send]
  external toggleComment:
    (
      editor,
      ~from: Position.t=?,
      ~to_: Position.t=?,
      ~options: commentOptions=?,
      unit
    ) =>
    unit =
    "";
  /* Set the lines in the given range to be line comments. Will fall back to blockComment when no line comment style is defined for the mode. */
};
