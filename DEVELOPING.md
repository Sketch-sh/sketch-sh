This is a mono repository that hold all of ReasonML playground code.

# General coding style

I use refmt with default width (80 chars). By running `npm install` inside client directory, a git hook will be setuped for you to auto format on staged files.

You can clone this project by running this command:

```sh
https://github.com/thangngoc89/rtop-ui
```

# Frontend

## Folder structures

```
└── client
    ├── bsconfig.json             # Bucklescript's configuration
    ├── entry.js                  # Webpack entry point
    ├── graphql_schema.json       # graphql_ppx stored schema file for validating queries
    ├── public                    # public assets
    ├── shared                    # bindings, shared utilities
    ├── src                       # main app, routing, handling authentication, ...
    ├── src_analyzer              # web worker: analyzing and executing code
    └── src_editor                # editor: containing code of editor related code
```

## Developing frontend

- cd into client and install dependencies

```sh
~/rtop-ui: > cd client && npm install
```

- Run an initial bsb build

```sh
~/rtop-ui/client: > npm run bs:build
```

- Start webpack development server

```sh
~/rtop-ui/client: > npm start
```

- Run Bucklescript in watch mode (another tab)

```sh
~/rtop-ui/client: > npm run bs:start
```

> Tips: You don't need to do this if you're using VSCode with VSCode ReasonML plugin
> just hit save and bsb will compile everything for you
> More information here: https://github.com/reasonml-editor/vscode-reasonml/#bsb

- To make an production bundle:

```sh
~/rtop-ui/client: > npm run build
```

# Server

Server implementations contains 2 microserives: auth, hasura's graphql engine

## Folder structures

```
└── server
   ├── auth                         # auth microserives
   ├── hasura                       # hasura's metadata
   ├── schema                       # PostgreSQL database migrations
   ├── docker-compose.yaml          # Docker Compose for easy development setup
   └── Makefile                     # Makefile for running commands quickly
```

## Installation

// TODO: deploy a staging server for local development

- You need to have these installed

  - `docker`: // TODO: find a link for installing docker
  - `docker-compose`: Head over to [docker-compose's release page](https://github.com/docker/compose/releases). Choose the latest release and follow the instruction.

- Github OAuth apps

  - Go to https://github.com/settings/developers
  - Create a new app
  - Set `Authorization callback URL` to `http://localhost:3001/auth/github/callback`
  - Save `Client ID` and `Client Secret`

## Starting development server

- Copying `.env.example` to `.env`

```sh
~/rtop-ui/server: > cp .env.example .env
```

- Open it `.env` and add `GITHUB_CLIENT_ID` and `GITHUB_CLIENT_SECRET`

- Start docker-compose in detach mode

```sh
~/rtop-ui/server: > && docker-compose up -d
```

- Migrate to latest database schema

```sh
~/rtop-ui/server: > make migrate
```

### View server logs

```sh
~/rtop-ui/server: > docker-compose logs -f
```
