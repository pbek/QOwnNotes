#!/usr/bin/env bash
###
# If the app is built in CLion it needs to be wrapped to not get the error:
# > Warning: Could not find the Qt platform plugin "xcb"
###

rm .QOwnNotes-wrapped* || true
nix-shell -p kdePackages.wrapQtAppsHook qt6.qtbase --run "wrapQtApp QOwnNotes"
./QOwnNotes "$@"
exit_status=$?
rm ./QOwnNotes

exit $exit_status
