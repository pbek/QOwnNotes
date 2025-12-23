# Dockerfile to create image for static webpage
# VuePress 2 + Vuetify 3

FROM node:22-alpine as node

WORKDIR /app

# Copy package files first for better layer caching
COPY package*.json ./

# Install dependencies with legacy peer deps flag for VuePress 2
RUN npm install --legacy-peer-deps

# Copy the rest of the application
COPY . .

# Build the static site
RUN ./scripts/run-build.sh

FROM nginx:latest
COPY --from=node /app/src/.vuepress/dist /usr/share/nginx/html
COPY org.flathub.VerifiedApps.txt /usr/share/nginx/html/.well-known/org.flathub.VerifiedApps.txt
COPY nginx.conf /etc/nginx/conf.d/default.conf
