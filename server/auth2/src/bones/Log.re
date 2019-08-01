[@bs.module "./logger.js"] external debug: 'a => unit = "info";
[@bs.module "./logger.js"] external debug2: ('a, 'b) => unit = "debug";
[@bs.module "./logger.js"] external debug3: ('a, 'b, 'c) => unit = "debug";
[@bs.module "./logger.js"] external debug4: ('a, 'b, 'c, 'd) => unit = "debug";

[@bs.module "./logger.js"] external info: 'a => unit = "info";
[@bs.module "./logger.js"] external info2: ('a, 'b) => unit = "info";
[@bs.module "./logger.js"] external info3: ('a, 'b, 'c) => unit = "info";

[@bs.module "./logger.js"] external warn: 'a => unit = "warn";
[@bs.module "./logger.js"] external warn2: ('a, 'b) => unit = "warn";
[@bs.module "./logger.js"] external warn3: ('a, 'b, 'c) => unit = "warn";

[@bs.module "./logger.js"] external error: 'a => unit = "error";
[@bs.module "./logger.js"] external error2: ('a, 'b) => unit = "error";
[@bs.module "./logger.js"] external error3: ('a, 'b, 'c) => unit = "error";

[@bs.module "./logger.js"]
external middleware:
  (~addContextFromRequest: Express.Request.t => Js.t('a)=?, unit) =>
  Express.Middleware.t =
  "middleware";
