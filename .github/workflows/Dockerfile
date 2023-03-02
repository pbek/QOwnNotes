# Build: docker build -t qownnotes-ubuntu18-dev .
# Run: docker run -it --rm qownnotes-ubuntu18-dev bash

FROM ubuntu:bionic

ENV LANG C.UTF-8
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y software-properties-common

# Install current version of git for the checkout action
RUN apt-add-repository ppa:git-core/ppa -y

# Install current version of python for the setup-python action
# https://launchpad.net/~deadsnakes/+archive/ubuntu/ppa
RUN apt-add-repository ppa:deadsnakes/ppa -y

RUN apt-get update && apt-get install -y \
    git wget curl findutils sudo snapd snapcraft build-essential libc6 \
    python3.11 \
    libgl1-mesa-dev libpulse-dev libxcb-glx0 libxcb-icccm4 libxcb-image0 \
    libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-render0 \
    libxcb-shape0 libxcb-shm0 libxcb-sync1 libxcb-util1 libxkbcommon-x11-0 libxcb-xfixes0 \
    libxcb-xinerama0 libxcb1 libxkbcommon-x11-0 \
    && rm -rf /var/lib/apt/lists/*

RUN git --version
RUN rm /usr/bin/python3 && ln -s /usr/bin/python3.11 /usr/bin/python3
RUN python3 --version
#RUN python3 -m pip --version
#RUN python3 -m ensurepip --upgrade
RUN curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
RUN python3 get-pip.py

#RUN python3 -m venv myenv

# https://stackoverflow.com/a/69573368/1581487
RUN /usr/bin/python3 -m pip install -U setuptools

RUN /usr/bin/python3 -m pip install wheel py7zr==0.19.* aqtinstall==2.0.0
#RUN /usr/bin/python3 -m pip install aqtinstall==2.0.0
