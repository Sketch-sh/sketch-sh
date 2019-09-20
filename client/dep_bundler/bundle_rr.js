const fs = require("fs");
const path = require("path");
const { resolve } = path;
const serialize_binary = require("./serialize_binary");

function walk(dir) {
  return new Promise((resolve, reject) => {
    fs.readdir(dir, (error, files) => {
      if (error) {
        return reject(error);
      }
      Promise.all(
        files.map(file => {
          return new Promise((resolve, reject) => {
            const filepath = path.join(dir, file);
            fs.stat(filepath, (error, stats) => {
              if (error) {
                return reject(error);
              }
              if (stats.isDirectory()) {
                walk(filepath).then(resolve);
              } else if (stats.isFile()) {
                resolve(filepath);
              }
            });
          });
        })
      ).then(foldersContents => {
        resolve(
          foldersContents.reduce(
            (all, folderContents) => all.concat(folderContents),
            []
          )
        );
      });
    });
  });
}

function uppercase_first_char(string) {
  return string.charAt(0).toUpperCase() + string.slice(1);
}
function lowercase_first_char(string) {
  return string.charAt(0).toLowerCase() + string.slice(1);
}

let change_ext = (a, new_ext) => {
  let ext = path.extname(a);
  return a.replace(ext, "") + "." + new_ext;
};

let get_module_name = a => {
  let file_name_without_ext = path.basename(a).replace(path.extname(a), "");
  return uppercase_first_char(file_name_without_ext);
};

let node_modules = resolve(__dirname, "../node_modules");

let main = async (pkg_name = "reason-react") => {
  let pkg_binary_artifact_path = resolve(node_modules, pkg_name, "lib/bs/src");
  let pkg_src_path = resolve(node_modules, pkg_name, "src");

  let result_path = resolve(__dirname, pkg_name);
  let bs_js_path = resolve(result_path, `bs_${pkg_name}.js`);

  console.log("Making new output dir ", result_path);
  // Ensure path
  try {
    fs.mkdirSync(result_path);
  } catch (e) {}

  let find_modules = await walk(pkg_src_path);

  let modules = new Set();

  find_modules.forEach(file_path => {
    if (file_path.endsWith(".re") || file_path.endsWith(".ml")) {
      modules.add(file_path.replace(pkg_src_path, ""));
    }
  });

  let arfifacts = [...modules].map(relative_module_path => {
    let cmi = path.join(
      pkg_binary_artifact_path,
      change_ext(relative_module_path, "cmi")
    );
    let cmj = path.join(
      pkg_binary_artifact_path,
      change_ext(relative_module_path, "cmj")
    );

    let js = path.join(pkg_src_path, change_ext(relative_module_path, "js"));

    let has_artifact = fs.existsSync(cmi) && fs.existsSync(cmj);

    if (!has_artifact) {
      throw new Error(
        "Can't not find build arfifact for " + relative_module_path
      );
    }

    let module_name = get_module_name(relative_module_path);

    return {
      js,
      cmi,
      cmj,
      module_name,
    };
  });

  console.log("Write bucklescript build artifact to ", bs_js_path);
  fs.writeFileSync(bs_js_path, "");

  arfifacts.forEach(module => {
    const cmi = module.cmi;
    const cmiBasename = path.basename(cmi);
    fs.appendFileSync(
      bs_js_path,
      `ocaml.load_module("/static/cmis/${cmiBasename}", `
    );
    serialize_binary(cmi, bs_js_path);
    const cmj = module.cmj;
    const cmjBasename = path.basename(cmj);
    fs.appendFileSync(bs_js_path, `, "${cmjBasename}", `);
    serialize_binary(cmj, bs_js_path);
    fs.appendFileSync(bs_js_path, ");\n");

    let js_dest = path.join(
      result_path,
      lowercase_first_char(path.basename(module.js))
    );
    console.log("Copy", module.js, js_dest);
    fs.copyFileSync(module.js, js_dest);
  });
};

main().then(() => console.log("Done"));
