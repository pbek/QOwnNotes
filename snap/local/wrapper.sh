#!/bin/env bash

pwd
find $SNAP
desktop-launch $SNAP/usr/bin/QOwnNotes -style=Breeze --snap "$@"
# desktop-launch $SNAP/QOwnNotes -style=Breeze --snap "$@"
