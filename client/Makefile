export PATH := ./node_modules/.bin:$(PATH)

project_name = sketch-client
DUNE = opam exec -- dune
opam_file = $(project_name).opam

.DEFAULT_GOAL := help

.PHONY: help
help: ## Print this help message
	@echo "List of available make commands";
	@echo "";
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}';
	@echo "";

.PHONY: create-switch
create-switch:
	opam switch create . 4.06.1 --deps-only

.PHONY: init
init: install pins ## Configure everything to develop this repository in local
	yarn

.PHONY: pins
pins: ## Pin development dependencies
	opam pin add $(project_name).dev .

.PHONY: install
install: ## Install development dependencies
	opam install . --deps-only --with-test

.PHONY: deps
deps: $(opam_file) ## Alias to update the opam file and install the needed deps

.PHONY: format
format: ## Format the codebase with ocamlformat
	$(DUNE) build @fmt --auto-promote

.PHONY: format-check
format-check: ## Checks if format is correct
	$(DUNE) build @fmt

.PHONY: watch
watch: ## Watch for the filesystem and rebuild on every change
	yarn bs:start

.PHONY: start
start: ## Serve the application with a local HTTP server
	yarn start

.PHONY: build
build: ## Build the project
	NODE_ENV=build webpack && \
	cp public/* build

.PHONY: clean
clean: ## Clean build artifacts and other generated files
	yarn bs:clean

$(opam_file): $(project_name).opam.template dune-project ## Update the package dependencies when new deps are added to dune-project
	opam exec -- dune build @install        # Update the $(project_name).opam file
