type console;

type log;
type log_encoded;

[@bs.module "console-feed"]
external hook: (console, log_encoded => unit) => unit = "Hook";

[@bs.module "console-feed"] external decode: log_encoded => log = "Decode";

module Display = {
  [@bs.module "console-feed"] [@react.component]
  external make:
    (~logs: array(log), ~variant: string=?, ~styles: Js.t('styles)=?) =>
    React.element =
    "Console";
};
