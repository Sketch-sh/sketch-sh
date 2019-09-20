# Sketch's server management guide

## Resources

- https://blog.machinebox.io/deploy-machine-box-in-digital-ocean-385265fbeafd
- Using pass for password management: https://www.passwordstore.org/

## Setup a new machine

- Create a new VPS on DO via docker-machinemachine on DO

```sh
docker-machine create \
	--driver digitalocean \
	--digitalocean-size "s-1vcpu-1gb" \
	--digitalocean-image "ubuntu-18-04-x64" \
	--digitalocean-access-token $(pass do/api_token) \
	--digitalocean-region "nyc1" \
	--digitalocean-tags "sketch_sh" \
	--digitalocean-private-networking \
	sketch-prod-1
```

- If this is a first node, make it a swarm master

```
docker-machine ssh sketch-prod-1
docker swarm init --advertise-addr private_ip_address
```

- Setup a network interface for traefik

```sh
docker network create --driver=overlay traefik-public
```

## Deployment

- Go to `/server`
- Build and push images to docker registry

```
docker-compose -f docker-compose.prod.yml build && \
docker-compose -f docker-compose.prod.yml push
```

- Make sure you have setup `.env.production` correctly
- Deploy the stack

```sh
./deploy.sh
```

If you see `permission denied error`, you need to make `deploy.sh` executable:

```sh
chmox +x ./deploy.sh
```

## Postgres management

### Create new user

DO Postgres cluster comes with `doadmin` user, we need to create a non privileged for normal operation.

- Remember to add current IP to accessable IP list in DO's dashboard

```
export DATABASE_URL=postgresql_access_string_of_doadmin
```

- Login to pgsql

```
pgsql $DATABASE_URL
```

> Tips:
> You can use docker to install and access to pgsql

```
docker run --rm -d --name pg-docker postgres
docker exec -it pg-docker psql $DATABASE_URL
```

- Generate a new password

```
pass generate --no-symbols do/sketch_postgres_passwd
```

- Create new user and database inside psql

```
CREATE USER sketch WITH PASSWORD 'password_here';
CREATE DATABASE sketch;
GRANT ALL PRIVILEGES ON DATABASE sketch to sketch;
```

### Backup and restore from Heroku

- Go inside `pg-docker` for access to `pg_dump` and `pg_restore`

```
docker exec -it pg-docker bash
```

- Backup with pg_dump

```
pg_dump -d CONNECTION_STRING_HERE -f BACKUP_FILE_NAME.dump -Fc
```

- Restore with pg_restore

> Note: When restoring from Heroku database, there will be an error about pgcrypto extension
> Just ignore it for now

```
pg_restore -d CONNECTION_STRING_HERE --no-privileges --no-owner BACKUP_FILE_NAME.dump
```

## Certificate management

For simplicity, I use Cloudflare Origin CA + Full (Strict) SSL mode.

- Generate a new Cloudflare Origin CA

Go to CF's Dashboard > Crypto > Origin Certificates > Create Certificate

Keep default configs and save the public key + private key as `cloudflare.cert` and `cloudflare.key`.

- Add these configs into Docker Secrets

Make sure you are connecting with Swarm master node.

```
docker secrets create cloudflare.cert ./cloudflare.cert
docker secerts create cloudflare.key ./cloudflare.key
```

Now you can mount these secerts into `traefik`'s container under `/run/secerts`

(check `docker-compose.prod.yml`)

# Generating RS512 key for JWT tokens

```sh
ssh-keygen -t rsa -b 4096 -m PEM -f jwt.key
openssl rsa -in jwt.key -pubout -outform PEM -out jwt.pub
```
