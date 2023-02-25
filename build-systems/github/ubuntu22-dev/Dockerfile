# Build: docker build -t qownnotes-ubuntu22-dev .
# Run: docker run -it --rm qownnotes-ubuntu22-dev bash

FROM ubuntu:22.04

ENV LANG C.UTF-8
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y software-properties-common

# Install current version of git for the checkout action
RUN apt-add-repository ppa:git-core/ppa -y

RUN apt-get update && apt-get install -y \
    git wget curl findutils sudo build-essential libc6 \
    qt6-base-dev qt6-declarative-dev libqt6svg6-dev libqt6websockets6-dev libqt6svg6 \
    libqt6svgwidgets6 libqt6websockets6 libqt6core5compat6 libqt6core5compat6-dev \
    libgl1-mesa-dev libpulse-dev libxcb-glx0 libxcb-icccm4 libxcb-image0 \
    libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-render0 \
    libxcb-shape0 libxcb-shm0 libxcb-sync1 libxcb-util1 libxcb-xfixes0 \
    libxcb-xinerama0 libxcb1 libxkbcommon-x11-0 \
    && rm -rf /var/lib/apt/lists/*

RUN git --version
