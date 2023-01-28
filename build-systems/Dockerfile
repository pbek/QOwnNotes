# Local release environment
FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get -y install bash tmuxinator \
         build-essential autoconf automake autotools-dev dh-make debhelper \
         devscripts fakeroot xutils lintian pbuilder cdbs gnupg \
         python3 python3-pip osc xz-utils git rsync qttools5-dev-tools \
         qt5-qmake qtbase5-dev libqt5svg5-dev qtdeclarative5-dev \
         libqt5websockets5-dev libqt5x11extras5-dev

# QOwnNotes repo is supposed to be mounted to /QOwnNotes
RUN useradd -ms /bin/bash release
USER release
WORKDIR /home/release
# osc needs m2crypto:
RUN pip3 install m2crypto
RUN mkdir /home/release/.ssh -m 0700
RUN git config --global user.email "patrizio@bekerle.com" && git config --global user.name "Patrizio Bekerle"
