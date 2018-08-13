ROOT_PATH := $(shell pwd)

prod-auth:
	cd server/auth && \
	now -A ${ROOT_PATH}/now/auth.prod.now.json && \
	now -A ${ROOT_PATH}/now/auth.prod.now.json alias 

prod-client:
	# Deploying frontend - production
	now && now alias sketch.now.sh && \
	now alias sketch.sh --rules ${ROOT_PATH}/now/frontend.prod.rules.json

.PHONY: prod-auth prod-client
