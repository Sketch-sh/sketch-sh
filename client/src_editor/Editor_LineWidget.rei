type cb = unit => unit;
let createErrorWidget: string => Dom.element;
let createWarningWidget: string => Dom.element;
let createValueWidget: string => (Dom.element, cb => unit);
let createStdoutWidget: string => Dom.element;
