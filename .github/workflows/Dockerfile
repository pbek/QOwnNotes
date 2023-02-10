
FROM ubuntu:bionic

ENV LANG C.UTF-8
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y software-properties-common

# Install current version for the checkout action
RUN apt-add-repository ppa:git-core/ppa -y

RUN apt-get update && apt-get install -y \
    git wget curl findutils sudo snapd snapcraft build-essential libc6 \
    python3 python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN git --version
