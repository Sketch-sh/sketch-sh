export PATH := ./node_modules/.bin:$(PATH)

bswatch: 
	bsb -w
start:
	webpack-dev-server
build:
	NODE_ENV=build webpack && \
	cp public/* build

.PHONY: start build bswatch
