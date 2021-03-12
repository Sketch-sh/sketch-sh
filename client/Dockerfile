# build environment
FROM node:12 as builder
WORKDIR /app
COPY package.json yarn.lock ./
RUN yarn install
COPY . .
RUN yarn build && yarn cache clean

# # server
FROM nginx:1.18-alpine
ARG APP_ENV
COPY --from=builder /app/build /bin/www
COPY nginx.$APP_ENV.conf /etc/nginx/conf.d/default.conf
EXPOSE 80
CMD [ "nginx", "-g", "daemon off;" ]