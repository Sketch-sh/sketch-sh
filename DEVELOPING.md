This is a mono repository that holds all of Sketch.sh code.

# General coding style

I use refmt with default width (80 chars). By running `yarn install` inside client directory, a git hook will be setup for you to auto format on staged files.

You can clone this project by running this command:

```sh
git clone https://github.com/sketch-sh/sketch-sh
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
~/sketch-sh: > cd client && yarn install
```

- Run an initial bsb build

```sh
~/sketch-sh/client: > yarn bs:build
```

- Start webpack development server

```sh
~/sketch-sh/client: > yarn start
```

- Run Bucklescript in watch mode (another tab)

```sh
~/sketch-sh/client: > yarn bs:start
```

- To make a production bundle:

```sh
~/sketch-sh/client: > yarn build
```

# Server

Server implementation contains 2 microservices: auth, hasura's graphql engine

## Folder structures

```
└── client
   └── nginx.conf                   # nginx proxy config (will serve static files but also proxy requests to services below)
└── server
   ├── auth                         # auth microservice
   ├── hasura                       # hasura's metadata
   ├── schema                       # PostgreSQL database migrations
   └── Makefile                     # Makefile for running commands quickly
└── docker-compose.yaml             # Docker Compose for easy development setup
```

## Installation

- You need to have these installed
  - `docker`: Find installation instructions for your platform from [docker's docs](https://docs.docker.com/install/#supported-platforms).
  - `docker-compose`: Head over to [docker-compose's release page](https://github.com/docker/compose/releases). Choose the latest release and follow the instruction.

- Github OAuth apps

  - Go to https://github.com/settings/developers
  - Create a new app
  - Set `Authorization callback URL` to `http://localhost:3000/api/auth/github/callback` (for usage with Webpack dev server) or `http://localhost:4000/api/auth/github/callback` (for usage with local nginx proxy to emulate production setup)
  - Save `Client ID` and `Client Secret`

## Starting development server

- Copy `.env.example` to `.env`

```sh
~/sketch-sh/server: > cp .env.example .env
```

- Open `.env` and add `AUTH_GITHUB_CLIENT_ID` and `AUTH_GITHUB_CLIENT_SECRET`

- Start docker-compose in detach mode

```sh
~/sketch-sh/server: > make dev
```

### View server logs

```sh
~/sketch-sh/server: > docker-compose logs -f
```
