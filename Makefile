client-bswatch: 
	$(MAKE) -C client bswatch
client-start:
	$(MAKE) -C client start
client-build:
	$(MAKE) -C client build

server-start: 
	$(MAKE) -C server start
server-watch:
	$(MAKE) -C server watch
server-bswatch:
	$(MAKE) -C server bswatch

.PHONY: client-bswatch client-start client-webpack server-start server-watch server-bswatch
