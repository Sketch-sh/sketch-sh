migrate:
	cd schema && \
	./node_modules/.bin/knex migrate:latest && \
	cd ../hasura && \
	hasura metadata apply --access-key hasura
save:
	cd hasura && \
	hasura metadata export --access-key hasura
update-schema: save
	cd ../client && \
	./node_modules/.bin/send-introspection-query http://localhost:8080/v1alpha1/graphql -H "X-Hasura-Access-Key: hasura"

clear:
	docker-compose down -v && \
	docker-compose up -d

reset: update-schema clear migrate

STAGING_DATABASE_URL := $(shell heroku config:get DATABASE_URL -a rtop-server)?ssl=true
PROD_DATABASE_URL := $(shell heroku config:get DATABASE_URL -a sketch-graphql)?ssl=true

staging-migrate: 
	cd schema && \
	DATABASE_URL=$(STAGING_DATABASE_URL) ./node_modules/.bin/knex migrate:latest --env production && \
	cd ../hasura && \
	hasura metadata apply --endpoint https://rtop-server.herokuapp.com --access-key $$RTOP_ACCESS_KEY

prod-migrate: 
	cd schema && \
	DATABASE_URL=$(PROD_DATABASE_URL) ./node_modules/.bin/knex migrate:latest --env production && \
	cd ../hasura && \
	hasura metadata apply --endpoint https://sketch-graphql.herokuapp.com --access-key $$SKETCH_ACCESS_KEY

psql:
	# Access psql of a running container
	docker exec -it server_postgres_1 psql -U postgres
dev:
	docker-compose -f docker-compose.yml -f docker-compose.dev.yml up -d
dev-build:
	docker-compose -f docker-compose.yml -f docker-compose.dev.yml build

do-build:
	docker-compose -f docker-compose.prod.yml build && docker-compose -f docker-compose.prod.yml push
do-ps:
	docker stack ps sketch
do-deploy:
	export $(cat .env.production) && docker stack deploy --with-registry-auth --compose-file=docker-compose.prod.yml sketch
do-logs:
	docker services 

.PHONY: migrate \
				save \
				update-schema \
				clear reset prod-migrate staging-migrate dev \
				do-build do-ps do-deploy do-logs
