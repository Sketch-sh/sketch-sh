migrate:
	cd schema && \
	./node_modules/.bin/knex migrate:latest && \
	cd ../hasura && \
	hasura metadata apply --admin-secret hasura
save:
	cd hasura && \
	hasura metadata export --admin-secret hasura
update-schema: save
	cd ../client && \
	./node_modules/.bin/send-introspection-query http://localhost:8080/v1alpha1/graphql -H "X-Hasura-Access-Key: hasura"

reset: update-schema clear migrate

do-build:
	docker-compose -f docker-compose.prod.yml build && docker-compose -f docker-compose.prod.yml push
do-ps:
	docker stack ps sketch
do-deploy:
	export $(cat .env.production) && docker stack deploy --with-registry-auth --compose-file=docker-compose.prod.yml sketch
do-logs:
	docker services 

.PHONY: migrate save update-schema \
				reset \
				do-build do-ps do-deploy do-logs \

