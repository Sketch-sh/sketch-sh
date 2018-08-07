[@bs.val] external require: string => unit = "require";
[@bs.val] external import: string => Js.Promise.t('a) = "import";
