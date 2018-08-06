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
	./node_modules/.bin/send-introspection-query http://localhost:8081/v1alpha1/graphql -H "X-Hasura-Access-Key: hasura"

clear: 
	docker-compose down -v && \
	docker-compose up -d

reset: update-schema clear migrate

.PHONY: migrate save update-schema clear reset
