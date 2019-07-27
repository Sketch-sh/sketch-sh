// Reference: https://github.com/Microsoft/monaco-editor/blob/master/monaco.d.ts

module MarkerSeverity = {
  [@bs.deriving {jsConverter: newType}]
  type t =
    | [@bs.as 1] Hint
    | [@bs.as 2] Info
    | [@bs.as 4] Warning
    | [@bs.as 8] Error;
};

module IRange = {
  type t = {
    .
    "startLineNumber": int,
    "startColumn": int,
    "endLineNumber": int,
    "endColumn": int,
  };

  [@bs.obj]
  external make:
    (
      ~startLineNumber: int,
      ~startColumn: int,
      ~endLineNumber: int,
      ~endColumn: int
    ) =>
    t =
    "";
};

module IModelContentChange = {
  type t = {
    .
    // The range that got replaced.
    "range": IRange.t,
    // The offset of the range that got replaced.
    "rangeOffset": int,
    // The length of the range that got replaced.
    "rangeLength": int,
    // The new text for the range.
    "text": string,
  };
};

module IModelContentChangedEvent = {
  type t = {
    .
    "changes": array(IModelContentChange.t),
    // The (new) end-of-line character.
    "eol": string,
    // The new version id the model has transitioned to.
    "versionId": int,
    // Flag that indicates that this event was generated while undoing.
    "isUndoing": bool,
    // Flag that indicates that this event was generated while redoing.
    "isRedoing": bool,
    // Flag that indicates that all decorations were lost with this edit.
    // The model has been reset to a new value.
    "isFlush": bool,
  };
};

module IMarkerData = {
  [@bs.deriving abstract]
  type t = {
    [@bs.optional]
    code: string,
    severity: MarkerSeverity.abs_t,
    message: string,
    [@bs.optional]
    source: string,
    startLineNumber: int,
    startColumn: int,
    endLineNumber: int,
    endColumn: int,
    // relatedInformation?: IRelatedInformation[];
    // tags?: MarkerTag[];
  };
  let make = t;
};

module ITextModel = {
  type t;
};

module IModelDecorationOptions = {
  [@bs.deriving abstract]
  type t = {
    // stickiness?: TrackedRangeStickiness,
    [@bs.optional]
    className: string,
    // glyphMarginHoverMessage?: IMarkdownString | IMarkdownString[] | null;
    // hoverMessage?: IMarkdownString | IMarkdownString[] | null;
    [@bs.optional]
    isWholeLine: bool,
    [@bs.optional]
    zIndex: int,
    // overviewRuler?: IModelDecorationOverviewRulerOptions | null;
    // glyphMarginClassName?: string | null;
    [@bs.optional]
    linesDecorationsClassName: string,
    // marginClassName?: string | null;
    // inlineClassName?: string | null;
    // inlineClassNameAffectsLetterSpacing?: boolean;
    // beforeContentClassName?: string | null;
    // afterContentClassName?: string | null;
  };
  let make = t;
};

module IModelDeltaDecoration = {
  [@bs.deriving abstract]
  type t = {
    range: IRange.t,
    options: IModelDecorationOptions.t,
  };
  let make = t;
};

module IViewZone = {
  type id;

  [@bs.deriving abstract]
  type t = {
    [@bs.optional]
    afterColumn: int,
    afterLineNumber: int,
    domNode: Dom.element,
    [@bs.optional]
    heightInLines: int,
    [@bs.optional]
    marginDomNode: Dom.element,
    [@bs.optional]
    minWidthInPx: string,
    [@bs.optional]
    onComputedHeight: int => unit,
    [@bs.optional]
    onDomNodeTop: int => unit,
    [@bs.optional]
    suppressMouseDown: bool,
  };

  let make = t;
};

module IViewZoneChangeAccessor = {
  type t = {
    .
    // Create a new view zone
    [@bs.meth] "addZone": IViewZone.t => IViewZone.id,
    // Remove a zone
    [@bs.meth] "removeZone": IViewZone.id => unit,
    // Change a zone's position.
    // The editor will rescan the `afterLineNumber` and `afterColumn` properties of a view zone.
    [@bs.meth] "layoutZone": IViewZone.id => unit,
  };
};

module IStandaloneCodeEditor = {
  type t = {
    .
    [@bs.meth] "getModel": unit => ITextModel.t,
    [@bs.meth]
    "deltaDecorations":
      (array(string), array(IModelDeltaDecoration.t)) => array(string),
    [@bs.meth] "changeViewZones": (IViewZoneChangeAccessor.t => unit) => unit,
  };
};

module Editor = {
  type t = {
    .
    [@bs.meth]
    "setModelMarkers": (ITextModel.t, string, array(IMarkerData.t)) => unit,
  };
};

module Languages = {
  [@bs.deriving abstract]
  type languageExtensionPoint = {
    id: string,
    [@bs.optional]
    extensions: array(string),
    [@bs.optional]
    filenames: array(string),
    [@bs.optional]
    filenamePatterns: array(string),
    [@bs.optional]
    firstLine: string,
    [@bs.optional]
    aliases: array(string),
    [@bs.optional]
    mimetypes: array(string),
  };

  type t = {. [@bs.meth] "register": languageExtensionPoint => unit};
};

type monaco = {
  .
  "languages": Languages.t,
  "editor": Editor.t,
};

[@bs.module]
external monaco: monaco = "monaco-editor/esm/vs/editor/editor.api";
let monaco = monaco;
