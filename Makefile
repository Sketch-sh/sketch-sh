ROOT_PATH := $(shell pwd)

prod-auth:
	cd server/auth && \
	now -A ${ROOT_PATH}/now/auth.prod.now.json && \
	now -A ${ROOT_PATH}/now/auth.prod.now.json alias 

prod-client:
	# Deploying frontend - production
	now && now alias sketch.now.sh && \
	now alias sketch.sh --rules ${ROOT_PATH}/now/frontend.prod.rules.json

stag-auth:
	cd server/auth && \
	now -A ${ROOT_PATH}/now/auth.stag.now.json && \
	now -A ${ROOT_PATH}/now/auth.stag.now.json alias 

stag-client:
	# Deploying frontend - production
	now && now alias sketch-staging.now.sh && \
	now alias staging.sketch.sh --rules ${ROOT_PATH}/now/frontend.stag.rules.json

.PHONY: prod-auth prod-client stag-auth stag-client
