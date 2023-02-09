FROM ubuntu:bionic

ENV LANG C.UTF-8
ENV DEBIAN_FRONTEND noninteractive

RUN add-apt-repository ppa:git-core/ppa

RUN apt-get update && apt-get install -y \
    git wget curl findutils \
    && rm -rf /var/lib/apt/lists/*

RUN git --version
