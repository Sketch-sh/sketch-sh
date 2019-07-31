# Engine bucklescript

## Build instruction

> Warning: It will get complicated pretty fast and I would love to move all of this into a reproducible Docker file

### Install needed dependencies

1. opam

More details: https://opam.ocaml.org/doc/Install.html

```sh
sh <(curl -sL https://raw.githubusercontent.com/ocaml/opam/master/shell/install.sh)
opam init
```

2. BuckleScript

Clone the BuckleScript fork that works with Sketch.sh. `bucklescript` root project folder
should be next to `sketch-sh` root project folder in the directory structure.

```sh
git clone https://github.com/thangngoc89/bucklescript/
git checkout sketch
```

js_of_ocaml is also needed to build the browser version of the BuckleScript compiler:

```sh
opam install js_of_ocaml
```

_Note: js_of_ocaml should be installed using opam switch 4.02.3._

```sh
git submodule update --init && node scripts/buildocaml.js # Build BuckleScript fork of OCaml
cd scripts
./ninja.js config && ./ninja.js build # Build BuckleScript
BS_PLAYGROUND=../../sketch-sh/client/public ./repl.js # Generate browser version of BuckleScript
```

If there are errors while building BuckleScript, try running `./scripts/ninja.js clean` and calling
`./ninja.js config && ./ninja.js build` again.

For convenience, the last two commands (`./ninja.js build` and `./repl.js`) are included in a script file, so when
new changes are done in BuckleScript code, one can just run `./repl.sh` from the `scripts` folder:

```sh
cd scripts && ./repl.sh
```

These steps should be enough to build BuckleScript for Sketch.sh. For more information
about how to work with BuckleScript, refer to [`bucklescript/CONTRIBUTING.md`](https://github.com/BuckleScript/bucklescript/blob/2e4c0558a8b417983a4c8ce8d6016ea3a4fa1fc5/CONTRIBUTING.md).

3. Reason (optional)

This step is optional, as a pre-packed version of `refmt.ml` is already included with Sketch.sh fork of BuckleScript.

Clone Reason (also next to `sketch-sh` and `bucklescript` folders in the directory structure):

```sh
git clone https://github.com/facebook/reason/
```

```sh
opam switch create 4.02.3
eval $(opam env)
cd reason
opam pin add -y reason .
opam pin add -y rtop .
npm install
```

IMPORTANT: the `bs-platform` version pinned in `package.json` ships wiht `bspack.exe`. It was removed in later release of `bs-platform`. If for some reason you need a newer `bs-platform`, you need to build bucklescript repository and grab the binary in `bucklescript/jscomp/bin/bspack.exe`

```sh
cd bspacks
version=3.5.0 bash reason_bspacks.sh
```

The script will failed because we don't have JVM installed but that isn't important. All we need is `refmt_api.ml` and `refmt_binary.ml`

```sh
# inside bspacks
cp ./build/refmt_api.ml ../../bucklescript/jscomp/main
```
