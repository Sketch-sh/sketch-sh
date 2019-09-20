import "@babel/polyfill";
import { Polestar, DefaultResolver } from "polestar";

let defaultResolver = new DefaultResolver();
const bs_stdlib = "http://localhost:3000/";

let polestar = new Polestar({
  globals: {
    process: {
      env: {},
    },
  },
  moduleThis: window,
  fetcher: async (url, meta) => {
    console.log("fetcher", url, meta);

    if (url === "vfs:///index.js") {
      return {
        url: url,
        id: url,
        dependencies: ["./stdlib/js_mapperRt.js"],
        code: `console.log("hey")`,
      };
    }

    console.log(url, meta);

    const { originalRequest, requiredById } = meta;

    if (originalRequest && originalRequest.startsWith("./stdlib/")) {
      let response = await fetch(bs_stdlib + originalRequest.slice(2));
      let code = await response.text();
      return {
        url: url,
        id: url,
        dependencies:[],
        code,
      };
    }
    return null;
  },
  resolver: defaultResolver,
  onEntry: (...args) => {
    console.log("onEntry", ...args);
  },
  onError: error => {
    console.error(error);
  },
});

async function main() {
  let indexModule = await polestar.require("react");
  console.log(indexModule);
}

main();
