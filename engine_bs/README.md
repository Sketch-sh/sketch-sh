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

2. Cloning repositories

```sh
git clone https://github.com/BuckleScript/bucklescript
git clone https://github.com/BuckleScript/bucklescript-playground
git clone https://github.com/facebook/reason/
```

3. Bootstraping

- reason

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

- bucklescript

Refer to `bucklescript/CONTRIBUTING.md`

```
git submodule update --init && node scripts/buildocaml.js
./scripts/ninja.js config && ./scripts/ninja.js build
./scripts/repl.js
```
