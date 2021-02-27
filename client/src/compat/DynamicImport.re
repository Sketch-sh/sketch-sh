/* Represent importable module a.k.a JavaScript object. */
type importable('a) = {
  .
  "__esModule": bool,
  "$$default": bool,
  [@bs.meth] "propertyIsEnumerable": string => bool,
};

/* Depack module (internal). */
external depack: importable('a) => 'a = "%identity";

/* Import module. */
[@bs.val] external import: string => Js.Promise.t(importable('a)) = "import";

/* Pipe (internal). */
let (||>) = (f, g, x) => g(f(x));

/* Map (auto-wrap). */
let (<$>) = (a, b) => Js.Promise.then_(b ||> Js.Promise.resolve, a);

/* Map catch (auto-wrap). */
let (<$!>) = (a, b) => Js.Promise.catch(b ||> Js.Promise.resolve, a);

/* Then. */
let (>>=) = (a, b) => Js.Promise.then_(b, a);

/* Catch. */
let (>>=!) = (a, b) => Js.Promise.catch(b, a);

/* Reverse then. */
let (=<<) = (a, b) => Js.Promise.then_(a, b);

/* Reverse catch. */
let (!=<<) = (a, b) => Js.Promise.catch(a, b);

/**
 * Convert regular Common.js/ES module (object) into acceptable module for compiler (array of value).
 * We have to care about ordering of value.
 *
 * When Reason/Ocaml module use a "let default" declaration, that will produce this output :
 *
 * --> COMMON.JS
 * exports.lazyValue = lazyValue;
 * exports.value = value;
 * exports.$$default = $$default;
 * exports.default = $$default;
 * exports.__esModule = true;
 *
 * --> ESM
 * export { lazyValue, value, $$default, $$default as default };
 *
 * Without default declaration, we get this :
 *
 * --> COMMON.JS
 * exports.lazyValue = lazyValue,
 * exports.value = value;
 *
 * --> ESM
 * export { lazyValue, value };
 *
 * -----------------------------------------------------------------------
 * => __esModule is provided on Common.js only if default export is used. Enumerable property.
 * => __esModule is flagged on ESM if you use compiler like Babel but thanks god, not enumerable property.
 * => We should remove "$$default" key to preserve module order on Common.js/ESM, "default" do the same job.
 * => We should remove "__esModule" key to preserve module order on Common.js because it's enumerable and that should not.
 * See : https://github.com/BuckleScript/bucklescript/issues/1987
 *
 * Module order is guaranteed by module interface and compiler inference.
**/
let depack = x => {
  if (x##propertyIsEnumerable("$$default")
      && x##propertyIsEnumerable("__esModule")
      &&
      x##__esModule) {
    [%raw {| delete x.__esModule |}] |> ignore;
  };
  [%raw {| delete x.$$default |}] |> ignore;
  depack(x);
};

/* Resolve one module. */
let resolve = fetch => fetch <$> depack;
