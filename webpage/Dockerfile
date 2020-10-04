# Dockerfile to create image for static webpage
FROM nginx:latest
COPY src/.vuepress/dist /usr/share/nginx/html
COPY nginx.conf /etc/nginx/conf.d/default.conf
