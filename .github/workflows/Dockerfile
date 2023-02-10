
FROM ubuntu:bionic

ENV LANG C.UTF-8
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y software-properties-common

# Install current version for the checkout action
RUN apt-add-repository ppa:git-core/ppa -y

RUN apt-get update && apt-get install -y \
    git wget curl findutils sudo snapd snapcraft build-essential libc6 \
    python3 python3-pip python3-setuptools python3-wheel python3-py7zr \
    libgl1-mesa-dev libpulse-dev libxcb-glx0 libxcb-icccm4 libxcb-image0 \
    libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-render0 \
    libxcb-shape0 libxcb-shm0 libxcb-sync1 libxcb-util1 libxcb-xfixes0 \
    libxcb-xinerama0 libxcb1 libxkbcommon-x11-0 \
    && rm -rf /var/lib/apt/lists/*

RUN /usr/bin/python3 -m pip install setuptools wheel py7zr==0.19.*

RUN git --version
