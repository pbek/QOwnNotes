#!/bin/sh

#
# does the coveralls submit under linux
#

sudo pip install cpp-coveralls
coveralls -b $(pwd)/unit_tests/ -r $(readlink -m ./../) -e tests/ -E .+/moc_.* -E .+/qrc_.* -E .+/qxt.* -E .+/ui_.* >coveralls.log
exit 0
