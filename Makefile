client-bswatch: 
	$(MAKE) -C client bswatch
client-start:
	$(MAKE) -C client start
client-build:
	$(MAKE) -C client build

.PHONY: client-bswatch client-start client-webpack
