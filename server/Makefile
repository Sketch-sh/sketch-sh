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

DATABASE_URL := $(shell heroku config:get DATABASE_URL -a rtop-server)?ssl=true

prod-migrate: 
	cd schema && \
	DATABASE_URL=$(DATABASE_URL) ./node_modules/.bin/knex migrate:latest --env production && \
	cd ../hasura && \
	hasura metadata apply --endpoint https://rtop-server.herokuapp.com --access-key $$RTOP_ACCESS_KEY

.PHONY: migrate save update-schema clear reset prod-migrate
