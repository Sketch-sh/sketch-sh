FROM node:10-alpine

# Install curl 
RUN apk add --no-cache curl postgresql
# Install hasura cli
RUN curl -L https://hasura.io/install/linux-amd64 -o hasura && \
  chmod +x hasura && \
  mv hasura /usr/bin/hasura

# Working on node
RUN mkdir -p /opt/schema
ARG NODE_ENV=production
ENV NODE_ENV $NODE_ENV
ARG PORT=80
WORKDIR /opt
COPY schema/package.json schema/package-lock.json ./
RUN npm install && npm cache clean --force
ENV PATH /opt/node_modules/.bin:$PATH
WORKDIR /opt/schema
COPY ./schema /opt/schema

COPY ./hasura /opt/hasura

CMD [ "./node_modules/.bin/knex", "migrate:latest" ]
