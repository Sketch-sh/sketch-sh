type console;

type raw_data;
type log;
type log_encoded;

[@bs.module "console-feed"]
external hook: (console, log_encoded => unit) => unit = "Hook";

[@bs.module "console-feed"] external decode: log_encoded => log = "Decode";

[@bs.module "console-feed/lib/Hook/parse/index.js"]
external parse: (string, raw_data) => log = "default";

[@bs.module "console-feed"] external encode: log => log_encoded = "Encode";

module Display = {
  [@bs.module "console-feed"] [@react.component]
  external make:
    (~logs: array(log), ~variant: string=?, ~styles: Js.t('styles)=?) =>
    React.element =
    "Console";
};
