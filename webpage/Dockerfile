# Dockerfile to create image for static webpage

FROM node:20-alpine as node
COPY . /app
WORKDIR /app
RUN npm install
RUN ./scripts/run-build.sh

FROM nginx:latest
COPY --from=node /app/src/.vuepress/dist /usr/share/nginx/html
COPY nginx.conf /etc/nginx/conf.d/default.conf
