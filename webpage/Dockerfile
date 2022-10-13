# Dockerfile to create image for static webpage

FROM node:12-alpine as node
COPY . /app
WORKDIR /app
RUN yarn install && yarn run build

FROM nginx:latest
COPY --from=node /app/src/.vuepress/dist /usr/share/nginx/html
COPY nginx.conf /etc/nginx/conf.d/default.conf
