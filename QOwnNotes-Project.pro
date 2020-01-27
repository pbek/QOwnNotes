TEMPLATE = subdirs

CONFIG += c++11

SUBDIRS = \
    src \
    tests

src.file = src/QOwnNotes.pro
tests.file = tests/QOwnNotesTests.pro

test.depends = src
 
