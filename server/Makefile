.PHONY: migrate
migrate:
	cd schema && \
	./node_modules/.bin/knex migrate:latest && \
	cd ../hasura && \
	hasura metadata apply --access-key hasura

.PHONY: save
save:
	cd hasura && \
	hasura metadata export --access-key hasura

.PHONY: update-schema
update-schema: save
	cd ../client && \
	./node_modules/.bin/send-introspection-query http://localhost:8080/v1alpha1/graphql -H "X-Hasura-Access-Key: hasura"

.PHONY: clear
clear:
	docker-compose -f docker-compose.yml -f docker-compose.dev.yml down -v && \
	docker-compose -f docker-compose.yml -f docker-compose.dev.yml up -d

.PHONY: reset
reset: update-schema clear migrate

.PHONY: psql
psql:
	# Access psql of a running container
	docker exec -it sketch-sh_postgres_1 psql -U postgres

.PHONY: dev
dev:
	cd .. && docker-compose -f docker-compose.yml -f docker-compose.dev.yml up -d

.PHONY: dev-build
dev-build:
	docker-compose -f docker-compose.yml -f docker-compose.dev.yml build

.PHONY: do-build
do-build:
	docker-compose -f docker-compose.yml -f docker-compose.prod.yml build && docker-compose -f docker-compose.yml -f docker-compose.prod.yml push
