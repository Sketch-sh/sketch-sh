# Prebuild docker image for now deployment
# - Prebuild bs-platform
# - Cache cypress's download
FROM node:10.8.0
RUN npm -g config set user root
RUN npm install -g bs-platform@4.0.3
WORKDIR ~/temp/
RUN echo "{}" > package.json && npm install cypress && \
  cd .. && rm -rf ~/temp/
