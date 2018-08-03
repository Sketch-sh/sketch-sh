migrate:
	cd schema && \
	./node_modules/.bin/knex migrate:latest && \
	cd ../hasura && \
	hasura metadata apply --access-key hasura

.PHONY: migrate
