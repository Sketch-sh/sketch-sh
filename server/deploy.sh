#/bin/bash -e

mktemp

hash () {
  md5 $1 | tr ' ' '\n' | head -1
}

hash_traefik=$(hash "./traefik.toml")

export $(cat .env.production)
TRAEFIK_CONF=$hash_traefik docker-compose -f docker-compose.prod.yml config > $tmp

echo "Written temporary configuration to $tmp"

eval $(docker-machine env sketch-prod-1)

echo "Switching docker_host to sketch-prod-1"

docker stack deploy -c $tmp sketch

echo "Deployed new stack"
