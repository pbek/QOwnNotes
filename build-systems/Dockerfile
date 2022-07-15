# Local release environment
FROM ubuntu:latest

RUN apt-get install bash tmuxinator \
         build-essential autoconf automake autotools-dev dh-make debhelper \
         devscripts fakeroot xutils lintian pbuilder cdbs \
         python3 python3-pip osc xz

# osc needs m2crypto:
RUN pip3 install m2crypto

# QOwnNotes repo is supposed to be mounted to /QOwnNotes
# A /config/.dput.cf needs to be mounted (usually from ~/.dput.cf)
# A /config/.oscrc needs to be mounted (usually from ~/.dput.cf)
#CMD ["/QOwnNotes/buildsystems/build-all.sh"]

USER 1000
