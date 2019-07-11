# hello-reason

[![Build Status](https://dev.azure.com/hi0906/Sketch/_apis/build/status/thangngoc89.sketch-v2?branchName=master)](https://dev.azure.com/hi0906/Sketch/_build/latest?definitionId=1&branchName=master)

A project which demonstrates a Reason workflow with [Esy][].

[Esy]: https://github.com/esy-ocaml/esy


## Usage

You need Esy, you can install the beta using [npm](https://npmjs.com):

    % npm install -g esy@latest

> NOTE: Make sure `esy --version` returns at least `0.5.4` for this project to build.

Then run the `esy` command from this project root to install and build depenencies.

    % esy

Now you can run your editor within the environment (which also includes merlin):

    % esy $EDITOR
    % esy vim

Alternatively you can try [vim-reasonml](https://github.com/jordwalke/vim-reasonml)
which loads esy project environments automatically.

After you make some changes to source code, you can re-run project's build
again with the same simple `esy` command.

    % esy

And test compiled executable (runs `scripts.tests` specified in
`package.json`):

    % esy test

Documentation for the libraries in the project can be generated with:

    % esy doc
    % esy open '#{self.target_dir}/default/_doc/_html/index.html'

Shell into environment:

    % esy shell


## Create Prebuilt Release:

`esy` allows creating prebuilt binary packages for your current platform, with
no dependencies.

    % esy npm-release
    % cd _release
    % npm publish
