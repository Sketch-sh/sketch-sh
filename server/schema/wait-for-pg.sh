#!/bin/bash
# wait-for-postgres.sh

echo $DATABASE_URL

set -e

host="$1"
shift
cmd="$@"

until psql $DATABASE_URL -c '\q'; do
  >&2 echo "Postgres is unavailable - sleeping"
  sleep 1
done

>&2 echo "Postgres is up - executing command"
exec $cmd
