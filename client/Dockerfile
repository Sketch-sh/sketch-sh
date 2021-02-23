# build environment
FROM node:12 as builder
WORKDIR /app
COPY package.json yarn.lock ./
RUN yarn install
COPY . ./
RUN yarn build

# # server
FROM nginx:stable-alpine
COPY --from=builder /app/build /bin/www
COPY nginx.conf /etc/nginx/conf.d/default.conf
EXPOSE 80
CMD [ "nginx", "-g", "daemon off;" ]