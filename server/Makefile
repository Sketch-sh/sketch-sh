start:
	node src/Server.bs.js
watch:
	./node_modules/.bin/nodemon -e js src/Server.bs.js
bswatch:
	./node_modules/.bin/bsb -w
bsb:
	./node_modules/.bin/bsb -clean-world && \
	./node_modules/.bin/bsb -make-world

db:
	./node_modules/.bin/knex migrate:rollback && \
	./node_modules/.bin/knex migrate:latest

.PHONY: start watch bswatch bsb db
