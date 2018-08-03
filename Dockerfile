FROM node:10
WORKDIR /usr/src
COPY client/package-lock.json client/package.json ./
RUN npm install
COPY client .
RUN npm run ci:build
RUN mv ./build /public
