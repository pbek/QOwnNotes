# Local release environment
FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

# Prepare install of gh
RUN (type -p wget >/dev/null || (apt update && apt-get install wget -y)) \
    && mkdir -p -m 755 /etc/apt/keyrings \
    && wget -qO- https://cli.github.com/packages/githubcli-archive-keyring.gpg | tee /etc/apt/keyrings/githubcli-archive-keyring.gpg > /dev/null \
    && chmod go+r /etc/apt/keyrings/githubcli-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | tee /etc/apt/sources.list.d/github-cli.list > /dev/null \

RUN apt-get update

# osc needs m2crypto:
RUN apt-get -y install bash \
         build-essential autoconf automake autotools-dev dh-make debhelper \
         devscripts fakeroot xutils lintian pbuilder cdbs gnupg \
         python3 python3-pip osc xz-utils git rsync qttools5-dev-tools \
         qt5-qmake qtbase5-dev libqt5svg5-dev qtdeclarative5-dev \
         libqt5websockets5-dev libqt5x11extras5-dev \
         qt6-base-dev qt6-declarative-dev libqt6svg6-dev libqt6websockets6-dev \
         libqt6svg6 libqt6svgwidgets6 libqt6websockets6 libqt6core5compat6 \
         libqt6core5compat6-dev gh curl python3-m2crypto \
         cmake pkg-config qt6-l10n-tools qt6-tools-dev qt6-tools-dev-tools

# Download and extract zellij
RUN curl -L https://github.com/zellij-org/zellij/releases/download/v0.42.2/zellij-x86_64-unknown-linux-musl.tar.gz -o zellij.tar.gz \
    && tar -xzvf zellij.tar.gz \
    && rm zellij.tar.gz \
    && chmod +x zellij \
    && mv zellij /usr/local/bin

# Try to fix https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=1074680
RUN sed -i 's/import imp/import importlib/' /usr/lib/python3/dist-packages/osc/commandline.py

# QOwnNotes repo is supposed to be mounted to /QOwnNotes
USER ubuntu
WORKDIR /home/ubuntu

RUN mkdir /home/ubuntu/.ssh -m 0700
RUN git config --global user.email "patrizio@bekerle.com" && git config --global user.name "Patrizio Bekerle"
