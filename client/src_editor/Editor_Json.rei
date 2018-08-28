open Editor_Types;

/*
 * This is the stored format in PostgreSQL database (a jsonb column)
 * {
 *   lang: "RE" | "ML"
 *   blocks: [
 *     {
 *       kind: "text"
 *       value: "This is my awesome Sketch"
 *     },
 *     {
 *       kind: "code"
 *       value: "let a: string = 1;"
 *     }
 *   ]
 * }
 */
module V1: {
  let decode: Js.Json.t => (lang, array(Link.link), array(Block.block));
  let encode: (lang, array(Link.link), array(Block.block)) => Js.Json.t;
};
