# This docker file must be in repository root folder 
# For now.sh Github integration to work
FROM thangngoc89/sketch-sh:now
WORKDIR /usr/src
COPY client/package-lock.json client/package.json ./
RUN npm link bs-platform
RUN npm install
COPY client .
RUN npm run ci:build
RUN mv ./build /public
